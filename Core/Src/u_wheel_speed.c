#include "u_wheel_speed.h"
#include "u_tx_general.h"
#include "main.h"
#include "tx_api.h"
#include <stdint.h>
#include <math.h>

#define WHEEL_SAMPLE_PERIOD_MS 25U
#define WHEEL_ZERO_TIMEOUT_MS  150U
#define PULSES_PER_ROTATION    60.0f
#define WHEEL_RADIUS_M         0.2032f /* 8-inch wheel radius */
#define WHEEL_CIRCUMFERENCE_M  (2.0f * (float)M_PI * WHEEL_RADIUS_M)

/* MPH = RPM × (2 × pi × WHEEL_RADIUS_M) × 60 / 1609.344 (meters per mile) */
#define RPM_TO_MPH (60.0f / 1609.344f)

static TIM_HandleTypeDef *htim_left;
static TIM_HandleTypeDef *htim_right;

static uint16_t left_previous_count;
static uint16_t right_previous_count;

static uint32_t previous_sample_tick;
static uint32_t left_last_pulse_tick;
static uint32_t right_last_pulse_tick;

static wheel_speed_data_t wheel_speed_data;

static uint16_t get_pulse_count(uint16_t current_count, uint16_t previous_count)
{
	uint16_t pulse_count;

	if (current_count >= previous_count) {
		pulse_count = current_count - previous_count;
	} else {
		pulse_count = (uint16_t)((UINT16_MAX - previous_count) +
					 current_count + 1U);
	}

	return pulse_count;
}

static void calculate_wheel_speed(uint16_t pulse_count, uint32_t elapsed_ms,
				  float *rpm, float *mph)
{
	float frequency_hz;

	if (elapsed_ms == 0U) {
		return;
	}

	frequency_hz = ((float)pulse_count * 1000.0f) / (float)elapsed_ms;
	*rpm = (frequency_hz * 60.0f) / PULSES_PER_ROTATION;
	*mph = *rpm * WHEEL_CIRCUMFERENCE_M * RPM_TO_MPH;
}

void wheel_speed_init(TIM_HandleTypeDef *_htim_left,
		      TIM_HandleTypeDef *_htim_right)
{
	htim_left = _htim_left;
	htim_right = _htim_right;

	__HAL_TIM_SET_COUNTER(htim_left, 0U);
	__HAL_TIM_SET_COUNTER(htim_right, 0U);

	left_previous_count = 0U;
	right_previous_count = 0U;

	previous_sample_tick = (uint32_t)tx_time_get();
	left_last_pulse_tick = previous_sample_tick;
	right_last_pulse_tick = previous_sample_tick;

	wheel_speed_data.left_rpm = 0.0f;
	wheel_speed_data.left_mph = 0.0f;
	wheel_speed_data.right_rpm = 0.0f;
	wheel_speed_data.right_mph = 0.0f;
}

void wheel_pulse_check(void)
{
	uint32_t current_tick = (uint32_t)tx_time_get();
	uint32_t elapsed_ms = TICKS_TO_MS(current_tick - previous_sample_tick);

	if (elapsed_ms < WHEEL_SAMPLE_PERIOD_MS) {
		return;
	}

	uint16_t left_current_count =
		(uint16_t)__HAL_TIM_GET_COUNTER(htim_left);
	uint16_t right_current_count =
		(uint16_t)__HAL_TIM_GET_COUNTER(htim_right);

	uint16_t left_pulse_count =
		get_pulse_count(left_current_count, left_previous_count);
	uint16_t right_pulse_count =
		get_pulse_count(right_current_count, right_previous_count);

	left_previous_count = left_current_count;
	right_previous_count = right_current_count;
	previous_sample_tick = current_tick;

	if (left_pulse_count > 0U) {
		calculate_wheel_speed(left_pulse_count, elapsed_ms,
				      &wheel_speed_data.left_rpm,
				      &wheel_speed_data.left_mph);

		left_last_pulse_tick = current_tick;
	} else if (TICKS_TO_MS(current_tick - left_last_pulse_tick) >=
		   WHEEL_ZERO_TIMEOUT_MS) {
		wheel_speed_data.left_rpm = 0.0f;
		wheel_speed_data.left_mph = 0.0f;
	}

	if (right_pulse_count > 0U) {
		calculate_wheel_speed(right_pulse_count, elapsed_ms,
				      &wheel_speed_data.right_rpm,
				      &wheel_speed_data.right_mph);

		right_last_pulse_tick = current_tick;
	} else if (TICKS_TO_MS(current_tick - right_last_pulse_tick) >=
		   WHEEL_ZERO_TIMEOUT_MS) {
		wheel_speed_data.right_rpm = 0.0f;
		wheel_speed_data.right_mph = 0.0f;
	}
}

wheel_speed_data_t wheel_speed_get_data(void)
{
	return wheel_speed_data;
}
