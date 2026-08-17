#include "u_wheel_speed.h"
#include "u_tx_general.h"
#include "main.h"
#include "tx_api.h"
#include <stdint.h>
#include <stdbool.h>

#define WHEEL_ZERO_TIMEOUT_MS 150U
#define WHEEL_PERIOD_SAMPLES  4U
#define TIMER_FREQUENCY_HZ    100000U
#define PULSES_PER_ROTATION   60U

typedef struct {
	uint16_t previous_capture;
	uint16_t period_history[WHEEL_PERIOD_SAMPLES];
	uint32_t period_sum;
	volatile uint32_t average_period_ticks;
	volatile uint32_t update_count;
	uint8_t period_index;
	uint8_t period_count;
	bool capture_valid;
	volatile bool rearm_capture;
} wheel_capture_t;

static TIM_HandleTypeDef *htim_left;
static TIM_HandleTypeDef *htim_right;

static wheel_capture_t left_capture;
static wheel_capture_t right_capture;

static uint32_t left_processed_update_count;
static uint32_t right_processed_update_count;

static uint32_t left_last_pulse_tick;
static uint32_t right_last_pulse_tick;

static wheel_speed_data_t wheel_speed_data;

/**
 * @brief Reset the stored pulse-period samples for one wheel.
 *
 * @param capture Wheel capture state.
 */
static void reset_period_history(wheel_capture_t *capture)
{
	uint8_t index;

	capture->period_sum = 0U;
	capture->average_period_ticks = 0U;
	capture->period_index = 0U;
	capture->period_count = 0U;

	for (index = 0U; index < WHEEL_PERIOD_SAMPLES; index++) {
		capture->period_history[index] = 0U;
	}
}

/**
 * @brief Reset all capture state for one wheel.
 *
 * @param capture Wheel capture state.
 */
static void reset_capture_state(wheel_capture_t *capture)
{
	capture->previous_capture = 0U;
	capture->update_count = 0U;
	capture->capture_valid = false;
	capture->rearm_capture = false;

	reset_period_history(capture);
}

/**
 * @brief Calculate the timer period between two capture events.
 *
 * @param current_capture  Current timer capture value.
 * @param previous_capture Previous timer capture value.
 *
 * @return Period between captures in timer ticks.
 */
static uint16_t get_capture_period(uint16_t current_capture,
				   uint16_t previous_capture)
{
	uint16_t period_ticks;

	if (current_capture >= previous_capture) {
		period_ticks = current_capture - previous_capture;
	} else {
		// Account for timer counter rollover.
		period_ticks =
			(UINT16_MAX - previous_capture) +
			current_capture + 1U;
	}

	return period_ticks;
}

/**
 * @brief Store a pulse period and update the moving average.
 *
 * @param capture      Wheel capture state.
 * @param period_ticks New pulse period in timer ticks.
 */
static void store_period(wheel_capture_t *capture, uint16_t period_ticks)
{
	if (capture->period_count < WHEEL_PERIOD_SAMPLES) {
		// Fill the sample history before using it as a circular buffer.
		capture->period_history[capture->period_index] =
			period_ticks;

		capture->period_sum += (uint32_t)period_ticks;
		capture->period_count++;
	} else {
		// Replace the oldest sample while maintaining the running sum.
		capture->period_sum -=
			(uint32_t)capture->
				period_history[capture->period_index];

		capture->period_history[capture->period_index] =
			period_ticks;

		capture->period_sum += (uint32_t)period_ticks;
	}

	capture->period_index++;

	if (capture->period_index >= WHEEL_PERIOD_SAMPLES) {
		// Wrap the circular-buffer index.
		capture->period_index = 0U;
	}

	capture->average_period_ticks =
		capture->period_sum /
		(uint32_t)capture->period_count;

	capture->update_count++;
}

/**
 * @brief Process one wheel input-capture event.
 *
 * @param capture         Wheel capture state.
 * @param current_capture Current timer capture value.
 */
static void process_capture(wheel_capture_t *capture,
			    uint16_t current_capture)
{
	uint16_t period_ticks;

	if (capture->rearm_capture) {
		// Discard stale samples and use this pulse as the new reference.
		reset_period_history(capture);

		capture->previous_capture = current_capture;
		capture->capture_valid = true;
		capture->rearm_capture = false;
	} else if (!capture->capture_valid) {
		// Two captures are required before a period can be calculated.
		capture->previous_capture = current_capture;
		capture->capture_valid = true;
	} else {
		period_ticks =
			get_capture_period(current_capture,
					   capture->previous_capture);

		capture->previous_capture = current_capture;

		if (period_ticks > 0U) {
			store_period(capture, period_ticks);
		}
	}
}

/**
 * @brief Read the latest average pulse period.
 *
 * @param capture                Wheel capture state.
 * @param processed_update_count Last processed update count.
 * @param average_period_ticks   Destination for the average pulse period.
 *
 * @return true if a new period is available, otherwise false.
 */
static bool get_average_period(const wheel_capture_t *capture,
			       uint32_t *processed_update_count,
			       uint32_t *average_period_ticks)
{
	uint32_t update_count_before;
	uint32_t update_count_after;
	bool new_period_available;

	do {
		// Retry if the ISR updates the average during the read.
		update_count_before = capture->update_count;

		*average_period_ticks =
			capture->average_period_ticks;

		update_count_after = capture->update_count;
	} while (update_count_before != update_count_after);

	if (update_count_after != *processed_update_count) {
		*processed_update_count = update_count_after;
		new_period_available = true;
	} else {
		new_period_available = false;
	}

	return new_period_available;
}

/**
 * @brief Convert the average pulse period to wheel RPM.
 *
 * @param average_period_ticks Average period between sensor pulses.
 *
 * @return Wheel speed in RPM.
 */
static uint16_t calculate_wheel_rpm(uint32_t average_period_ticks)
{
	uint32_t timer_ticks_per_minute;
	uint32_t ticks_per_wheel_rotation;
	uint32_t rpm;
	uint16_t rpm_result;

	rpm_result = 0U;

	if (average_period_ticks > 0U) {
		timer_ticks_per_minute = TIMER_FREQUENCY_HZ * 60U;

		// Convert the pulse period into one full wheel rotation.
		ticks_per_wheel_rotation =
			average_period_ticks *
			PULSES_PER_ROTATION;

		// Round the calculated RPM to the nearest whole number.
		rpm = (timer_ticks_per_minute +
		       (ticks_per_wheel_rotation / 2U)) /
		      ticks_per_wheel_rotation;

		if (rpm > UINT16_MAX) {
			rpm_result = UINT16_MAX;
		} else {
			rpm_result = (uint16_t)rpm;
		}
	}

	return rpm_result;
}

void wheel_speed_init(TIM_HandleTypeDef *_htim_left,
		      TIM_HandleTypeDef *_htim_right)
{
	uint32_t current_tick;

	htim_left = _htim_left;
	htim_right = _htim_right;

	reset_capture_state(&left_capture);
	reset_capture_state(&right_capture);

	__HAL_TIM_SET_COUNTER(htim_left, 0U);
	__HAL_TIM_SET_COUNTER(htim_right, 0U);

	left_processed_update_count = 0U;
	right_processed_update_count = 0U;

	current_tick = (uint32_t)tx_time_get();

	left_last_pulse_tick = current_tick;
	right_last_pulse_tick = current_tick;

	wheel_speed_data.left_rpm = 0U;
	wheel_speed_data.right_rpm = 0U;

	(void)HAL_TIM_IC_Start_IT(htim_left, TIM_CHANNEL_1);
	(void)HAL_TIM_IC_Start_IT(htim_right, TIM_CHANNEL_1);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint16_t current_capture;

	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		current_capture =
			(uint16_t)HAL_TIM_ReadCapturedValue(
				htim, TIM_CHANNEL_1);

		if (htim->Instance == htim_left->Instance) {
			process_capture(&left_capture, current_capture);
		} else if (htim->Instance == htim_right->Instance) {
			process_capture(&right_capture, current_capture);
		} else {
			// Capture belongs to another timer.
		}
	}
}

void wheel_pulse_check(void)
{
	uint32_t current_tick;
	uint32_t average_period_ticks;

	current_tick = (uint32_t)tx_time_get();

	if (get_average_period(&left_capture,
			       &left_processed_update_count,
			       &average_period_ticks)) {
		wheel_speed_data.left_rpm =
			calculate_wheel_rpm(average_period_ticks);

		left_last_pulse_tick = current_tick;
	} else if (TICKS_TO_MS(current_tick -
			       left_last_pulse_tick) >=
		   WHEEL_ZERO_TIMEOUT_MS) {
		if (wheel_speed_data.left_rpm > 0U) {
			// Rearm so the first returning pulse is only a reference.
			wheel_speed_data.left_rpm = 0U;
			left_capture.rearm_capture = true;
		}
	}

	if (get_average_period(&right_capture,
			       &right_processed_update_count,
			       &average_period_ticks)) {
		wheel_speed_data.right_rpm =
			calculate_wheel_rpm(average_period_ticks);

		right_last_pulse_tick = current_tick;
	} else if (TICKS_TO_MS(current_tick -
			       right_last_pulse_tick) >=
		   WHEEL_ZERO_TIMEOUT_MS) {
		if (wheel_speed_data.right_rpm > 0U) {
			// Rearm so the first returning pulse is only a reference.
			wheel_speed_data.right_rpm = 0U;
			right_capture.rearm_capture = true;
		}
	}
}

wheel_speed_data_t wheel_speed_get_data(void)
{
	return wheel_speed_data;
}
