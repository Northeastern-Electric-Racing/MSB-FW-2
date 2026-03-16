#include "u_wheel_speed.h"
#include "u_can.h"
#include "u_queues.h"

static TIM_HandleTypeDef  *htim_left;
static TIM_HandleTypeDef  *htim_right;

/* Left Wheel */
static uint32_t left_val1 = 0;
static uint32_t left_val2 = 0;
static uint8_t  left_captured = 0;
static float left_rpm = 0;

/* Right Wheel */
static uint32_t right_val1 = 0;
static uint32_t right_val2 = 0;
static uint8_t  right_captured = 0;
static float right_rpm = 0;

void wheel_speed_init(TIM_HandleTypeDef *_htim_left, TIM_HandleTypeDef *_htim_right) {
    htim_left = _htim_left;
    htim_right = _htim_right;

    HAL_TIM_IC_Start_IT(htim_left, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(htim_right, TIM_CHANNEL_1);
}

void wheel_speed_capture_callback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == htim_left->Instance && htim->Channel  == HAL_TIM_ACTIVE_CHANNEL_1) {
        if (left_captured == 0) {
            left_val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            left_captured = 1;
        }
        else
        {
            left_val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            uint32_t diff;
            
            if (left_val2 > left_val1) {
                diff = left_val2 - left_val1
            }
            else {
                diff = 0xFFFFFFFF - left_val1 + left_val2
            }

            float frequency = (float)TIM_CLOCK_HZ / diff;
            left_rpm = (frequency * 60.0f) / PULSES_PER_ROTATION;

            __HAL_TIM_SET_COUNTER(htim, 0);
            left_captured = 0;
        }
    }

    if (htim->Instance == htim_right->Instance && htim->Channel  == HAL_TIM_ACTIVE_CHANNEL_1) {
        if (right_captured == 0) {
            right_val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            right_captured = 1;
        }
        else {
            right_val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            if (right_val2 > right_val1) {
                diff = right_val2 - right_val1
            }
            else {
                diff = 0xFFFFFFFF - right_val1 + right_val2
            }

            float frequency = (float)TIM_CLOCK_HZ / diff;
            right_rpm = (frequency * 60.0f) / PULSES_PER_ROTATION;

            __HAL_TIM_SET_COUNTER(htim, 0);
            right_captured = 0;
        }
    }
}

void send_wheel_speed() {
    struct __attribute__((__packed__)) {
        uint16_t right_rpm;
		uint16_t left_rpm;
	} wheel_speed_data;

    wheel_speed_data.right_rpm = (uint16_t)(right_rpm);
    wheel_speed_data.left_rpm = (uint16_t)(left_rpm);

    can_msg_t can_message = {.id = WHEEL_SPEED_CAN_ID, .len = 4, .data = {0}};

    memcpy(can_message.data, &wheel_speed_data, can_message.len);

    queue_send(&can_outgoing, &can_message, TX_NO_WAIT);
}