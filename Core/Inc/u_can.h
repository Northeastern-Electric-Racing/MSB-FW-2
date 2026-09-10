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

#define HDC2021_CAN_ID          0x602 // 0x622 for rear
#define IMU_ACCEL_CAN_ID        0x603 // 0x623 for rear
#define IMU_GYRO_CAN_ID         0x604 // 0x624 for rear
#define MAGNOMETER_CAN_ID       0x611 // 0x635 for rear 
#define ORIENTATION_CAN_ID      0x609 // 0x629 for rear 
#define VL53L7CX_CAN_ID         0x616 
#define SSC_CAN_ID              0x610 // 0x634 for rear
#define STRAIN_GAUGE_1_2_CAN_ID 0x605 // 0x625 for rear 
#define STRAIN_GAUGE_3_4_CAN_ID 0x612 // 0x636 for rear 
#define THERMOCOUPLE_CAN_ID     0x633
#define STEERING_ANGLE_CAN_ID   0x632
#define SHOCK_POTS_CAN_ID       0x606 // 0x626 for rear (front = right, rear = left?)
#define LOAD_CELLS_CAN_ID       0x630 
#define MISC_ADC1_CAN_ID        0x613 // 0x637 for rear 
#define MISC_ADC2_CAN_ID        0x614 // 0x638 for rear 
#define MISC_ADC3_CAN_ID        0x615 // 0x639 for rear
#define WHEEL_SPEED_CAN_ID      0x631



// left shockpot 0x626, right shockpot 0x606

// odyssey def says front ride height (0x607) and rear ride height (0x627) and 
// also front wheel temp (0x608) and rear wheel temp (0x628)

#endif /* u_can.h */

