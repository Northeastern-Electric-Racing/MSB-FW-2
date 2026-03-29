#ifndef U_WHEEL_SPEED_H
#define U_WHEEL_SPEED_H

#include "stm32h5xx_hal.h"

// Number of magnets on each wheel
#define MAGNETS_PER_REV  12

// Timer clock after prescaler (set this to match CubeMX)
// e.g. 200MHz / prescaler of 200 = 1MHz
#define TIM_CLOCK_HZ     1000000

void WheelSpeed_Init(TIM_HandleTypeDef *htim_fl, TIM_HandleTypeDef *htim_fr,
                     FDCAN_HandleTypeDef *hcan);

// Call from HAL_TIM_IC_CaptureCallback
void WheelSpeed_CaptureCallback(TIM_HandleTypeDef *htim);

// Call at your desired publish rate (e.g. 250Hz task)
void WheelSpeed_PublishCAN(void);

extern float fl_rpm;
extern float fr_rpm;

#endif /* u_wheel_speed.h */