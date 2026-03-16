#ifndef U_WHEEL_SPEED_H
#define U_WHEEL_SPEED_H

#include "stm32h5xx_hal.h"

// Number of pulses per wheel rotation
#define PULSES_PER_ROTATION  12

// Timer clock after prescaler (set this to match CubeMX)
// e.g. 200MHz / prescaler of 200 = 1MHz
#define TIM_CLOCK_HZ     1000000

void wheel_speed_init(TIM_HandleTypeDef *htim_left, TIM_HandleTypeDef *htim_right);

// Call from HAL_TIM_IC_CaptureCallback
void wheel_speed_capture_callback(TIM_HandleTypeDef *htim);

// Call at your desired publish rate (e.g. 250Hz task)
void send_wheel_speed();

extern float fl_rpm;
extern float fr_rpm;

#endif /* u_wheel_speed.h */