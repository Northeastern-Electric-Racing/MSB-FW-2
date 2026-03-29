#ifndef __U_SENSORS_H
#define __U_SENSORS_H

#include <stdint.h>

#define MOTION_FX_STATE_SIZE (size_t)(2432)

/*
 * Bias correction thresholds from MotionFX example implementations.
 * Lower GBIAS_* = more correction, higher = more stability.
 * Adjust if drift or instability occurs.
 */
#define GBIAS_ACC_TH_SC (2.0f * 0.000765f)
#define GBIAS_GYRO_TH_SC (2.0f * 0.002f)
#define GBIAS_MAG_TH_SC 0.0f

/*
 * DECIMATION controls how often sensor data is processed.
 * 1U = use every sample, higher = skip samples
 * Increase to ignore more samples if CPU usage is too high.
 */
#define DECIMATION 1U

/**
 * @brief initializes the lsm6dsv imu
 * @return whether there were errors initializing the lsm6dsv imu
 */
uint16_t init_imu();

/**
 * @brief initializes Magnetometer for reading
 * @return returns tx status for errors
 */
uint16_t init_magnetometer();

/**
 * @brief reads data from the lsm6dsv and lis2mdl, and performs motionfx
 * rotation calculations
 * @return whether there were errors in reading the data
 */
uint16_t read_imu_and_magnometer();

/**
 * @brief sends data from the lsm6dsv, lis2mdl, and motionfx rotation data over
 * CAN
 */
void send_imu_and_magnometer_data();

/**
 * @brief initializes the motion fx library
 */
void motion_fx_init();

/**
 * @brief zeros the motion fx rotation data
 */
void calibrate();

/**
 * @brief initializes the hdc2021 for reading temp and humidity
 * @return whether there were errors initializing the hdc2021
 */
uint16_t init_hdc2021();

/**
 * @brief tells the hdc2021 to read and prepare data in its registers
 * @return whether there were errors
 */
uint16_t prepare_data_hdc2021();

/**
 * @brief reads temp and humidity data from the hdc2021
 * @return whether there were errors in reading the data
 */
uint16_t read_hdc2021();

/**
 * @brief sends temp and humidity data from the hdc2021 over CAN
 */
void send_hdc2021_data();

/**
 * @brief sets up vl53l7cx time of flight sensor
 * @return if there were errors initalizing the sensor
 */
int32_t init_vl53l7cx();

/**
 * @brief Gets data from the vl53l7cx and updates a global variable accordingly
 * @returns if there were any errors in reading the data
 */
int32_t read_vl53l7cx();

/**
 * @brief sends time of flight data over CAN
 */
void send_vl53l7cx_data();

#endif /* u_sensors.h */
