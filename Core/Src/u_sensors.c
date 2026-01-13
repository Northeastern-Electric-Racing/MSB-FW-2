#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "u_can.h"
#include "main.h"
#include "u_tx_debug.h"
#include "u_tx_queues.h"
#include "u_queues.h"

#include "sht30.h"

#include "lsm6dsv_reg.h"

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;

static sht30_t sht30;
static stmdev_ctx_t imu;

/** 
 * IMU 
 */

 typedef struct {
    float x;
    float y;
    float z;
} LSM6DSV_Axes_t;

static int32_t _lsm6dsv_read(void *handle, uint8_t register_address, uint8_t *data, uint16_t length) {
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

static int32_t _lsm6dsv_write(void *handle, uint8_t register_address, uint8_t *data, uint16_t length) {
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

uint16_t imu_getAccelerometerData(LSM6DSV_Axes_t *axes) {
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

uint16_t imu_getGyroscopeData(LSM6DSV_Axes_t *axes) {
    int16_t buf[3];

    int status = lsm6dsv_angular_rate_raw_get(&imu, buf);

    if(status != 0) {
        PRINTLN_INFO("ERROR: Failed to call lsm6dso_angular_rate_raw_get() (Status: %d).", status);
        return U_ERROR;
    }

    axes->x = lsm6dsv_from_fs250_to_mdps(buf[0]);
    axes->y = lsm6dsv_from_fs250_to_mdps(buf[1]);
    axes->z = lsm6dsv_from_fs250_to_mdps(buf[2]);

    return U_SUCCESS;
}

uint16_t init_imu() {
    imu.read_reg = _lsm6dsv_read;
    imu.read_reg = _lsm6dsv_write;
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
    HAL_Delay(30); // This is probably overkill, but the datasheet lists the gyroscope's "Turn-on time" as 30ms, and I can't find anything else that specifies how long resets take.

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

static int16_t _float_to_int16(float value) {
    if (value > 32767.0f) {
        return 32767;
    }

    if (value < -32768.0f) {
        return -32768;
    }

    return (int16_t) value;
}

uint16_t read_imu() {
    LSM6DSV_Axes_t accel_axes;
    LSM6DSV_Axes_t gyro_axes;

    if (imu_getAccelerometerData(&accel_axes) != U_SUCCESS) {
        return U_ERROR;
    }

    if (imu_getGyroscopeData(&gyro_axes) != U_SUCCESS) {
        return U_ERROR;
    }

    struct __attribute__((__packed__)) {
		int16_t accel_x;
		int16_t accel_y;
		int16_t accel_z;
	} accel_data;

    accel_data.accel_x = _float_to_int16(accel_axes.x * 1000);
    accel_data.accel_y = _float_to_int16(accel_axes.y * 1000);
    accel_data.accel_z = _float_to_int16(accel_axes.z * 1000);

    struct __attribute__((__packed__)) {
		int16_t gyro_x;
		int16_t gyro_y;
		int16_t gyro_z;
	} gyro_data;

    gyro_data.gyro_x = _float_to_int16(gyro_axes.x * 1000);
    gyro_data.gyro_y = _float_to_int16(gyro_axes.y * 1000);
    gyro_data.gyro_z = _float_to_int16(gyro_axes.z * 1000);

    can_msg_t imu_accel_msg = { .id = IMU_ACCEL_CAN_ID,
				    .len = 6,
				    .data = { 0 } };
    
	can_msg_t imu_gyro_msg = { .id = IMU_GYRO_CAN_ID,
				   .len = 6,
				   .data = { 0 } };

    memcpy(imu_accel_msg.data, &accel_data, sizeof(accel_data));
    memcpy(imu_gyro_msg.data, &gyro_data, sizeof(gyro_data));

    queue_send(&can_outgoing, &imu_accel_msg, TX_NO_WAIT);
    queue_send(&can_outgoing, &imu_gyro_msg, TX_NO_WAIT);

    return U_SUCCESS;
}

/**
 * Sht30
 */

static uint8_t _sht30_i2c_write(uint8_t *data, uint8_t dev_address, uint8_t length){
    return HAL_I2C_Master_Transmit(&hi2c1, dev_address, data, length, HAL_MAX_DELAY);
}

static uint8_t _sht30_i2c_read(uint8_t *data, uint16_t command, uint8_t dev_address, uint8_t length) {
    return HAL_I2C_Mem_Read(&hi2c1, dev_address, command, sizeof(command), data, length, HAL_MAX_DELAY);
}

static uint8_t _sht30_i2c_blocking_read(uint8_t *data, uint16_t command, uint8_t dev_address, uint8_t length) {
    uint8_t command_buffer[2] = { (command & 0xff00u) >> 8u, command & 0xffu };
    _sht30_i2c_write(command_buffer, dev_address, sizeof(command_buffer));
    tx_thread_sleep(1);
    return HAL_I2C_Master_Receive(&hi2c1, dev_address, data, length, HAL_MAX_DELAY);
}

uint8_t init_sht30() {
    return sht30_init(&sht30, (Write_ptr) _sht30_i2c_write, (Read_ptr) _sht30_i2c_read, (Read_ptr) _sht30_i2c_blocking_read, SHT30_I2C_ADDR);
}

uint8_t read_sht30() {
    int status = sht30_get_temp_humid(&sht30);

    if (status) {
        PRINTLN_ERROR("ERROR: Failed to get sht30 data (Status: %d/%s).", status, hal_status_toString(status));
        return U_ERROR;
    }

    struct __attribute__((__packed__)) {
		float temp;
		float humidity;
	} data;

    data.temp = sht30.temp;
    data.humidity = sht30.humidity;

    can_msg_t can_message = { .id = SHT30_CAN_ID, .len = 8, .data = { 0 } };

    memcpy(can_message.data, &data, can_message.len);

    queue_send(&can_outgoing, &can_message, TX_NO_WAIT);

    return U_SUCCESS;
}