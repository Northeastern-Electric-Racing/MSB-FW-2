#pragma once

#include <stdint.h>

/**
 * @brief initializes the sht30 for reading temp and humidity
 * @return whether there were errors initializing the sht30
 */
uint8_t init_sht30();

/**
 * @brief reads and sends temp and humidity data from the sht30
 * @return whether there were errors in reading/sending the data
 */
uint8_t read_sht30();

/**
 * @brief initializes the lsm6dsv imu
 * @return whether there were errors initializing the lsm6dsv imu
 */
uint16_t init_imu();

/**
 * @brief reads and sends data from the lsm6dsv
 * @return whether there were errors in reading/sending the data
 */
uint8_t read_imu();