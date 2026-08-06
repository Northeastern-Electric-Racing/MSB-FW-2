#ifndef U_WHEEL_SPEED_H
#define U_WHEEL_SPEED_H

#include "stm32h5xx_hal.h"

/** Latest calculated wheel-speed measurements. */
typedef struct {
    uint16_t left_rpm;
    uint16_t right_rpm;
} wheel_speed_data_t;

/**
 * @brief Initializes the wheel speed timers
 * @param htim_left handle for left side wheel HAL effect sensor
 * @param htim_right handle for right side wheel HAL effect sensor
 */
void wheel_speed_init(TIM_HandleTypeDef *_htim_left, TIM_HandleTypeDef *_htim_right);

/**
 * @brief Samples both pulse counters and updates the wheel-speed measurements
 */
void wheel_pulse_check(void);

/**
 * @brief Returns the latest calculated wheel-speed measurements
 */
wheel_speed_data_t wheel_speed_get_data(void);

#endif /* u_wheel_speed.h */
