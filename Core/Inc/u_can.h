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

#define SHT30_CAN_ID        0xBAA
#define IMU_ACCEL_CAN_ID    0xBAB
#define IMU_GYRO_CAN_ID     0xBAC
#define MAGNOMETER_CAN_ID   0XBAD
#define ORIENTATION_CAN_ID  0XBAE

#endif /* u_can.h */