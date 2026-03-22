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

#define HDC2021_CAN_ID 0xBAF 
#define SHT30_CAN_ID 0xBAA       // 0xBBE for rear
#define IMU_ACCEL_CAN_ID 0xBAB   // 0xBBF for rear
#define IMU_GYRO_CAN_ID 0xBAC    // 0xBC0 for rear
#define MAGNOMETER_CAN_ID 0xBAD  // 0xBC1 for rear
#define ORIENTATION_CAN_ID 0xBAE // 0xBC2 for rear
#define VL53L7CX_CAN_ID 0xBAF    // 0xBC3 for rear, measures ride height

#endif /* u_can.h */
