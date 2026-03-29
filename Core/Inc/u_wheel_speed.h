#ifndef U_WHEEL_SPEED_H
#define U_WHEEL_SPEED_H

#include "stm32h5xx_hal.h"

/** Number of pulses per wheel rotation */
#define PULSES_PER_ROTATION 4

/** HAL Effect timers speed */
#define TIM_CLOCK_HZ        1000000

/**
 * @brief Initializes the wheel speed timers
 * @param htim_left handle for left side wheel HAL effect sensor
 * @param htim_right handle for right side wheel HAL effect sensor
 */
void wheel_speed_init(TIM_HandleTypeDef *_htim_left, TIM_HandleTypeDef *_htim_right);

/**
 * @brief Call from HAL_TIM_IC_CaptureCallback
 * @param htim timer from HAL_TIM_IC_CaptureCallback call
 */ 
void wheel_speed_capture_callback(TIM_HandleTypeDef *htim);

/**
 * @brief Calculates the wheel revolutions per minute based on the given frequency
 * @param frequency Frequency of pulses
 * @param rpm Pointer for storing calculated value
 */
void calculate_wheel_rpm(float frequency, float *rpm);

/**
 * Function checks whether the wheel pulse timers have expired. If so, it sets the rpm
 * for that wheel to 0.
 */
void wheel_pulse_check();

/**
 * Sends wheel speed data over CAN
 */
void send_wheel_speed();

#endif /* u_wheel_speed.h */