#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "u_sensors.h"
#include "u_can.h"
#include "main.h"
#include "u_tx_debug.h"
#include "u_tx_queues.h"
#include "u_queues.h"
#include "u_utils.h"
#include "motion_fx.h"

#include "sht30.h"
#include "lsm6dsv_reg.h"
#include "lis2mdl_reg.h"

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

static sht30_t sht30;
static stmdev_ctx_t imu;
static stmdev_ctx_t lis2mdl_ctx;

static MFX_knobs_t iKnobs;
static uint8_t mFXState[MOTION_FX_STATE_SIZE];

static float fx_rotation_x = 0.0f;
static float fx_rotation_y = 0.0f;
static float fx_rotation_z = 0.0f;

static float calibration_x = 0.0f;
static float calibration_y = 0.0f;
static float calibration_z = 0.0f;

static struct __attribute__((__packed__)) {
	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
} accel_data;

static struct __attribute__((__packed__)) {
	int16_t mag_x;
	int16_t mag_y;
	int16_t mag_z;
} mag_data;

static struct __attribute__((__packed__)) {
	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;
} gyro_data;

static struct __attribute__((__packed__)) {
	int16_t roll;
	int16_t pitch;
	int16_t yaw;
} orientation_data;

static struct __attribute__((__packed__)) {
	float temp;
	float humidity;
} sht30_data;


/** 
 * IMU 
 */



 typedef struct {
    float x;
    float y;
    float z;
} LSM6DSV_Axes_t;

int32_t _lsm6dsv_read(void *handle, uint8_t register_address, uint8_t *data, uint16_t length) {
    uint8_t spi_reg = (uint8_t)(register_address | 0x80);
    HAL_StatusTypeDef status;
    SPI_HandleTypeDef *spi_handle = (SPI_HandleTypeDef *) handle;
    
    status = HAL_SPI_Transmit(spi_handle, &spi_reg, sizeof(spi_reg), HAL_MAX_DELAY);
    if(status != HAL_OK) {
        PRINTLN_INFO("ERROR: Failed to send register address to lsm6dso over SPI (Status: %d/%s).", status, hal_status_toString(status));
        return -1;
    }

    status = HAL_SPI_Receive(spi_handle, data, length, HAL_MAX_DELAY);
    if(status != HAL_OK) {
        PRINTLN_INFO("ERROR: Failed to read from the lsm6dso over SPI (Status: %d/%s).", status, hal_status_toString(status));
        return -1;
    }
    
    return 0;
}

int32_t _lsm6dsv_write(void *handle, uint8_t register_address, uint8_t *data, uint16_t length) {
    uint8_t spi_reg = (uint8_t)(register_address & 0x7F);
    HAL_StatusTypeDef status;
    SPI_HandleTypeDef *spi_handle = (SPI_HandleTypeDef *) handle;
    
    status = HAL_SPI_Transmit(spi_handle, &spi_reg, sizeof(spi_reg), HAL_MAX_DELAY);
    if(status != HAL_OK) {
        PRINTLN_INFO("ERROR: Failed to send register address to lsm6dso over SPI (Status: %d/%s).", status, hal_status_toString(status));
        return -1;
    }
    
    status = HAL_SPI_Transmit(spi_handle, data, length, HAL_MAX_DELAY);
    if(status != HAL_OK) {
        PRINTLN_INFO("ERROR: Failed to write to the lsm6dso over SPI (Status: %d/%s).", status, hal_status_toString(status));
        return -1;
    }
    
    return 0;
}

void _delay(uint32_t delay) {
    return HAL_Delay(delay);
}

uint16_t imu_get_accelerometer_data(LSM6DSV_Axes_t *axes) {
    int16_t buf[3];

    int status = lsm6dsv_acceleration_raw_get(&imu, buf);

    if(status != 0) {
        PRINTLN_INFO("ERROR: Failed to call lsm6dsv_acceleration_raw_get() (Status: %d).", status);
        return U_ERROR;
    }

    axes->x = lsm6dsv_from_fs2_to_mg(buf[0]);
    axes->y = lsm6dsv_from_fs2_to_mg(buf[1]);
    axes->z = lsm6dsv_from_fs2_to_mg(buf[2]);

    return U_SUCCESS;
}

uint16_t imu_get_gyroscope_data(LSM6DSV_Axes_t *axes) {
    int16_t buf[3];

    int status = lsm6dsv_angular_rate_raw_get(&imu, buf);

    if(status != 0) {
        PRINTLN_INFO("ERROR: Failed to call lsm6dso_angular_rate_raw_get() (Status: %d).", status);
        return U_ERROR;
    }

    axes->x = lsm6dsv_from_fs2000_to_mdps(buf[0]);
    axes->y = lsm6dsv_from_fs2000_to_mdps(buf[1]);
    axes->z = lsm6dsv_from_fs2000_to_mdps(buf[2]);

    return U_SUCCESS;
}

uint16_t init_imu() {
    imu.read_reg = _lsm6dsv_read;
    imu.write_reg = _lsm6dsv_write;
    imu.mdelay = _delay;
    imu.handle = &hspi1;

    uint8_t id;
    uint8_t status = lsm6dsv_device_id_get(&imu, &id);

    if (status != 0) {
        PRINTLN_ERROR("Failed to call lsm6dsv_device_id_get() (Status: %d).", status);
        return U_ERROR;
    }

    if (id != LSM6DSV_ID) {
        PRINTLN_ERROR("lsm6dsv_device_id_get() returned an unexpected ID (id=%d, expected=%d). This means that the IMU is not configured correctly.", id, LSM6DSV_ID);
        return U_ERROR;
    }

    /* Reset IMU. */
    status = lsm6dsv_reset_set(&imu, LSM6DSV_GLOBAL_RST);
    if (status != 0) {
        PRINTLN_ERROR("Failed to reset the IMU via lsm6dsv_reset_set() (Status: %d).", status);
        return U_ERROR;
    }
    _delay(30); // This is probably overkill, but the datasheet lists the gyroscope's "Turn-on time" as 30ms, and I can't find anything else that specifies how long resets take.

    /* Enable Block Data Update. */
    status = lsm6dsv_block_data_update_set(&imu, PROPERTY_ENABLE); // Makes it so "output registers are not updated until LSB and MSB have been read". Datasheet says this is enabled by default but figured it was better to be explicit.
    if (status != 0) {
        PRINTLN_ERROR("Failed to enable Block Data Update via lsm6dsv_block_data_update_set() (Status: %d).", status);
        return U_ERROR;
    }

    /* Set Accelerometer Full Scale. */
    status = lsm6dsv_xl_full_scale_set(&imu, LSM6DSV_2g);
    if (status != 0) {
        PRINTLN_ERROR("Failed to set IMU Accelerometer Full Scale via lsm6dsv_xl_full_scale_set() (Status: %d).", status);
        return U_ERROR;
    }

    /* Set gyroscope full scale. */
    status = lsm6dsv_gy_full_scale_set(&imu, LSM6DSV_2000dps);
    if (status != 0) {
        PRINTLN_ERROR("Failed to set IMU Gyroscope Full Scale via lsm6dsv_gy_full_scale_set() (Status: %d).", status);
        return U_ERROR;
    }

    /* Set accelerometer output data rate. */
    status = lsm6dsv_xl_data_rate_set(&imu, LSM6DSV_ODR_AT_120Hz);
    if (status != 0) {
        PRINTLN_ERROR("Failed to set IMU Accelerometer Datarate via lsm6dsv_xl_data_rate_set() (Status: %d).", status);
        return U_ERROR;
    }

    /* Set gyroscope output data rate. */
    status = lsm6dsv_gy_data_rate_set(&imu, LSM6DSV_ODR_AT_120Hz);
    if (status != 0) {
        PRINTLN_ERROR("Failed to set IMU Gyroscope Datarate via lsm6dsv_gy_data_rate_set() (Status: %d).", status);
        return U_ERROR;
    }

    return U_SUCCESS;
}



/** 
 * COMPASS 
 */



int32_t _lis2mdl_read(void *handle, uint8_t register_address, uint8_t *data, uint16_t length) {
    uint8_t spi_reg = (uint8_t)(register_address | 0x80);
    HAL_StatusTypeDef status;
    
    /* Send the register address we're trying to read from. */
    status = HAL_SPI_Transmit((SPI_HandleTypeDef *) handle, &spi_reg, sizeof(spi_reg), HAL_MAX_DELAY);
    if (status != HAL_OK) {
        PRINTLN_ERROR("ERROR: Failed to send register address to lis2mdl over SPI (Status: %d/%s).", status, hal_status_toString(status));
        return -1;
    }
    
    /* Receive the data. */
    status = HAL_SPI_Receive((SPI_HandleTypeDef *) handle, data, length, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        PRINTLN_ERROR("ERROR: Failed to read from the lis2mdl over SPI (Status: %d/%s).", status, hal_status_toString(status));
        return -1;
    }
    
    return 0;
}

int32_t _lis2mdl_write(void *handle, uint8_t register_address, const uint8_t *data, uint16_t length){
    HAL_StatusTypeDef status;

    status = HAL_SPI_Transmit((SPI_HandleTypeDef *)handle, &register_address, sizeof(register_address), HAL_MAX_DELAY);
    if (status != HAL_OK) {
        PRINTLN_ERROR("ERROR: Failed to send register address to lis2mdl over SPI (Status: %d/%s).", status, hal_status_toString(status));
        return -1;
    }
    
    status = HAL_SPI_Transmit((SPI_HandleTypeDef *)handle, data, length, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        PRINTLN_ERROR("ERROR: Failed to write to the lis2mdl over SPI (Status: %d/%s).", status, hal_status_toString(status));
        return -1;
    }

    return 0;
}

uint16_t init_magnetometer() {
    uint8_t id;
    int32_t status;

    lis2mdl_ctx.handle = &hspi2;
    lis2mdl_ctx.read_reg = _lis2mdl_read;
    lis2mdl_ctx.write_reg = _lis2mdl_write;
    
    status = lis2mdl_device_id_get(&lis2mdl_ctx, &id);
    if (status != 0) {
        PRINTLN_ERROR("Failed to get LIS2MDL device ID (Status %d/%s)", status, hal_status_toString(status));
        return U_ERROR;
    }

    if (id != LIS2MDL_ID) {
        PRINTLN_ERROR("Device ID is not for LIS2MDL (ID: %d)", id);
        return U_ERROR;
    }

    status = lis2mdl_reset_set(&lis2mdl_ctx, 1);
    if (status != 0) {
        PRINTLN_ERROR("Failed to reset LIS2MDL (Status %d/%s)", status, hal_status_toString(status));
        return U_ERROR;
    }

    _delay(10);

    status = lis2mdl_operating_mode_set(&lis2mdl_ctx, LIS2MDL_CONTINUOUS_MODE);
    if (status != 0) {
        PRINTLN_ERROR("Failed to set LIS2MDL operating mode (Status %d/%s)", status, hal_status_toString(status));
        return U_ERROR;
    }
    

    status = lis2mdl_data_rate_set(&lis2mdl_ctx, LIS2MDL_ODR_50Hz);
    if (status != 0) {
        PRINTLN_ERROR("Failed to set LIS2MDL data rate (Status %d/%s)", status, hal_status_toString(status));
        return U_ERROR;
    }

    status = lis2mdl_offset_temp_comp_set(&lis2mdl_ctx, 1);
    if (status != 0) {
        PRINTLN_ERROR("Failed to enable LIS2MDL temp compensation (Status %d/%s)", status, hal_status_toString(status));
        return U_ERROR;
    }

    status = lis2mdl_block_data_update_set(&lis2mdl_ctx, 1);
    if (status != 0) {
        PRINTLN_ERROR("Failed to enable LIS2MDL block data update (Status %d/%s)", status, hal_status_toString(status));
        return U_ERROR;
    }

    return U_SUCCESS;
}



/** 
 * READ IMU AND MAGNOMETER 
 */



uint16_t read_imu_and_magnometer() {
    // Setup
    LSM6DSV_Axes_t accel_axes;
    LSM6DSV_Axes_t gyro_axes;
    int16_t mag_axes[3];
    uint8_t lis2mdl_data_ready;
    lis2mdl_mag_data_ready_get(&lis2mdl_ctx, &lis2mdl_data_ready);
    if (!lis2mdl_data_ready) {
        return U_SUCCESS;
    }

    // Read Data
    if (imu_get_accelerometer_data(&accel_axes) != U_SUCCESS) {
        return U_ERROR;
    }

    if (imu_get_gyroscope_data(&gyro_axes) != U_SUCCESS) {
        return U_ERROR;
    }

    if (lis2mdl_magnetic_raw_get(&lis2mdl_ctx, mag_axes) != 0) {
        return U_ERROR;
    }

    // Accel Data
    accel_data.accel_x = float_to_int16(accel_axes.x);
    accel_data.accel_y = float_to_int16(accel_axes.y);
    accel_data.accel_z = float_to_int16(accel_axes.z);

    // Gyro Data
    gyro_data.gyro_x = float_to_int16(gyro_axes.x);
    gyro_data.gyro_y = float_to_int16(gyro_axes.y);
    gyro_data.gyro_z = float_to_int16(gyro_axes.z);

    // Mag Data
    mag_data.mag_x = float_to_int16(lis2mdl_from_lsb_to_mgauss(mag_axes[0]));
    mag_data.mag_y = float_to_int16(lis2mdl_from_lsb_to_mgauss(mag_axes[1]));
    mag_data.mag_z = float_to_int16(lis2mdl_from_lsb_to_mgauss(mag_axes[2]));

    MFX_input_t mFXInput;
	MFX_output_t mFXOutput;

    // Acc (Convert mg to g)
    mFXInput.acc[0] = (float)(accel_axes.x / 1000.0f);
    mFXInput.acc[1] = (float)(accel_axes.y / 1000.0f);
    mFXInput.acc[2] = (float)(accel_axes.z / 1000.0f);

    // Gyro (Convert mdps to dps)
    mFXInput.gyro[0] = (float)(gyro_axes.x / 1000.0f);
    mFXInput.gyro[1] = (float)(gyro_axes.y / 1000.0f);
    mFXInput.gyro[2] = (float)(gyro_axes.z / 1000.0f);

    /* Magnetometer  
     * Convert milligauss to uT/50 (microtesla / 50)
     * 1 milligauss = 0.1 microTesla = 0.002 uT/50
     * So divide mG by 500 to get uT/50
     */
    mFXInput.mag[0] = (float)(lis2mdl_from_lsb_to_mgauss(mag_axes[0]) / 500.0f);
    mFXInput.mag[1] = (float)(lis2mdl_from_lsb_to_mgauss(mag_axes[1]) / 500.0f);
    mFXInput.mag[2] = (float)(lis2mdl_from_lsb_to_mgauss(mag_axes[2]) / 500.0f);

    float delta_time = 0.02f;

    MotionFX_propagate(mFXState, &mFXOutput, &mFXInput, &delta_time);
	MotionFX_update(mFXState, &mFXOutput, &mFXInput, &delta_time, NULL);

    fx_rotation_x = mFXOutput.rotation[0];
    fx_rotation_y = mFXOutput.rotation[1];
    fx_rotation_z = mFXOutput.rotation[2];

    orientation_data.yaw = float_to_int16(fx_rotation_x - calibration_x);
    orientation_data.pitch = float_to_int16(fx_rotation_y - calibration_y);
    orientation_data.roll = float_to_int16(fx_rotation_z - calibration_z);

    return U_SUCCESS;
}


void send_imu_and_magnometer_data() {
    can_msg_t imu_accel_msg = { 
        .id = IMU_ACCEL_CAN_ID,
		.len = 8,
		.data = { 0 } 
    };
    
	can_msg_t imu_gyro_msg = {
        .id = IMU_GYRO_CAN_ID,
		.len = 8,
		.data = { 0 } 
    };

    can_msg_t mag_msg = { 
        .id = MAGNOMETER_CAN_ID, 
        .len = 8, 
        .data = { 0 } 
    };

    can_msg_t orientation_msg = { 
        .id = ORIENTATION_CAN_ID, 
        .len = 8, 
        .data = { 0 } 
    };

    memcpy(imu_accel_msg.data, &accel_data, sizeof(accel_data));
    memcpy(imu_gyro_msg.data, &gyro_data, sizeof(gyro_data));
    memcpy(mag_msg.data, &mag_data, sizeof(mag_data));
    memcpy(orientation_msg.data, &orientation_data, sizeof(orientation_data));

    queue_send(&can_outgoing, &imu_accel_msg, TX_NO_WAIT);
    queue_send(&can_outgoing, &imu_gyro_msg, TX_NO_WAIT); 
    queue_send(&can_outgoing, &mag_msg, TX_NO_WAIT);
    queue_send(&can_outgoing, &orientation_msg, TX_NO_WAIT);
}



/**
 * MOTION FX
 */



void motion_fx_init() {
	if (MOTION_FX_STATE_SIZE < MotionFX_GetStateSize()) {
		PRINTLN_ERROR("Not enough memory allocated for MotionFX!");
		return;
	}

	MotionFX_initialize((MFXState_t *)mFXState);
	MotionFX_getKnobs(mFXState, &iKnobs);

	iKnobs.acc_orientation[0] = 's';
	iKnobs.acc_orientation[1] = 'e';
	iKnobs.acc_orientation[2] = 'u';

	iKnobs.gyro_orientation[0] = 's';
	iKnobs.gyro_orientation[1] = 'e';
	iKnobs.gyro_orientation[2] = 'u';

	iKnobs.mag_orientation[0] = 'n';
	iKnobs.mag_orientation[1] = 'e';
	iKnobs.mag_orientation[2] = 'u';

	iKnobs.gbias_acc_th_sc = GBIAS_ACC_TH_SC;
	iKnobs.gbias_gyro_th_sc = GBIAS_GYRO_TH_SC;
	iKnobs.gbias_mag_th_sc = GBIAS_MAG_TH_SC;

	iKnobs.output_type = MFX_ENGINE_OUTPUT_ENU;
	iKnobs.LMode = 1;
	iKnobs.modx = DECIMATION;

	MotionFX_setKnobs(mFXState, &iKnobs);
	MotionFX_enable_6X(mFXState, MFX_ENGINE_ENABLE);
}

void calibrate() {
    calibration_x = fx_rotation_x;
    calibration_y = fx_rotation_y;
    calibration_z = fx_rotation_z;
}



/**
 * SHT30
 */



uint8_t _sht30_i2c_write(uint8_t *data, uint8_t dev_address, uint8_t length){
    return HAL_I2C_Master_Transmit(&hi2c1, dev_address, data, length, HAL_MAX_DELAY);
}

uint8_t _sht30_i2c_read(uint8_t *data, uint16_t command, uint8_t dev_address, uint8_t length) {
    return HAL_I2C_Mem_Read(&hi2c1, dev_address, command, sizeof(command), data, length, HAL_MAX_DELAY);
}

uint8_t _sht30_i2c_blocking_read(uint8_t *data, uint16_t command, uint8_t dev_address, uint8_t length) {
    uint8_t command_buffer[2] = { (command & 0xff00u) >> 8u, command & 0xffu };
    _sht30_i2c_write(command_buffer, dev_address, sizeof(command_buffer));
    tx_thread_sleep(1);
    return HAL_I2C_Master_Receive(&hi2c1, dev_address, data, length, HAL_MAX_DELAY);
}

uint16_t init_sht30() {
    return sht30_init(&sht30, (Write_ptr) _sht30_i2c_write, (Read_ptr) _sht30_i2c_read, (Read_ptr) _sht30_i2c_blocking_read, SHT30_I2C_ADDR);
}

uint16_t read_sht30() {
    int status = sht30_get_temp_humid(&sht30);

    if (status) {
        PRINTLN_ERROR("ERROR: Failed to get sht30 data (Status: %d/%s).", status, hal_status_toString(status));
        return U_ERROR;
    }

    sht30_data.temp = sht30.temp;
    sht30_data.humidity = sht30.humidity;

    return U_SUCCESS;
}

void send_sht30_data() {
    can_msg_t can_message = { 
        .id = SHT30_CAN_ID, 
        .len = 8, 
        .data = { 0 } 
    };

    memcpy(can_message.data, &sht30_data, can_message.len);

    queue_send(&can_outgoing, &can_message, TX_NO_WAIT);
}