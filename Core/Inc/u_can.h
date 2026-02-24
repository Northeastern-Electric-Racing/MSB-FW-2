#ifndef __U_CAN_H
#define __U_CAN_H

#include <stdint.h>
#include "fdcan.h"

/**
 * @brief initializes FDCAN using the given FDCAN handle
 * 
 * @returns returns 0 on success, 1 on fail
 */
uint8_t can2_init(FDCAN_HandleTypeDef *hcan);

/* List of CAN interfaces */
extern can_t can2;

#define IMU_ACCEL_CAN_ID        0xBAB // 0xBBF for rear
#define IMU_GYRO_CAN_ID         0xBAC // 0xBC0 for rear
#define MAGNOMETER_CAN_ID       0xBB0 // 0xBC4 for rear
#define ORIENTATION_CAN_ID      0xBAE // 0xBC2 for rear
#define VL53L7CX_CAN_ID         0xBAF 
#define STRAIN_GAUGE_1_2_CAN_ID 0xDAA // 0xDBE for rear
#define STRAIN_GAUGE_3_4_CAN_ID 0xDAB // 0xDBF for rear
#define THERMOCOUPLE_CAN_ID     0xDAC
#define STEERING_ANGLE_CAN_ID   0xDAD
#define SHOCK_POTS_CAN_ID       0xDAE // 0xDC2 for rear
#define LOAD_CELLS_CAN_ID       0xDAF // 0xDC3 for rear
#define MISC_ADC1_CAN_ID        0xDB0 // 0xDC4 for rear
#define MISC_ADC2_CAN_ID        0xDB1 // 0xDC5 for rear
#define MISC_ADC3_CAN_ID        0xDB2 // 0xDC6 for rear

#endif /* u_can.h */
