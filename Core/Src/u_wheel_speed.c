#include "u_wheel_speed.h"
#include "u_tx_general.h"
#include "main.h"
#include "tx_api.h"
#include <stdint.h>
#include <math.h>

#define WHEEL_SAMPLE_PERIOD_MS 25U
#define WHEEL_ZERO_TIMEOUT_MS  150U
#define WHEEL_WINDOW_SAMPLES   4U
#define PULSES_PER_ROTATION    60.0f
#define WHEEL_RADIUS_M         0.2032f /* 8-inch wheel radius */
#define WHEEL_CIRCUMFERENCE_M  (2.0f * (float)M_PI * WHEEL_RADIUS_M)

/* MPH = RPM x circumference(m) x 60 / 1609.344 meters per mile. */
#define RPM_TO_MPH (60.0f / 1609.344f)

/**
 * @brief Rolling pulse-count window for one wheel.
 *
 * Stores the pulse count and elapsed time from the latest measurement
 * intervals. The totals are used to calculate the average pulse rate over
 * the complete window.
 */
typedef struct {
	uint16_t pulse_counts[WHEEL_WINDOW_SAMPLES];
	uint32_t sample_times_ms[WHEEL_WINDOW_SAMPLES];
	uint32_t total_pulses;
	uint32_t total_time_ms;
	uint8_t next_index;
	uint8_t sample_count;
} wheel_pulse_window_t;

static TIM_HandleTypeDef *htim_left;
static TIM_HandleTypeDef *htim_right;

static uint16_t left_previous_count;
static uint16_t right_previous_count;

static uint32_t previous_sample_tick;
static uint32_t left_last_pulse_tick;
static uint32_t right_last_pulse_tick;

static wheel_pulse_window_t left_pulse_window;
static wheel_pulse_window_t right_pulse_window;

static wheel_speed_data_t wheel_speed_data;

/**
 * @brief Calculate the number of pulses counted since the previous sample.
 *
 * @param current_count Current timer counter value.
 * @param previous_count Timer counter value from the previous sample.
 *
 * @return Number of pulses counted between the two counter readings.
 */
static uint16_t get_pulse_count(uint16_t current_count,
				uint16_t previous_count)
{
	uint16_t pulse_count;

	if (current_count >= previous_count) {
		pulse_count = current_count - previous_count;
	} else {
		pulse_count =
			(uint16_t)((UINT16_MAX - previous_count) +
				   current_count + 1U);
	}

	return pulse_count;
}

/**
 * @brief Reset a wheel pulse window.
 *
 * Clears all stored pulse counts, sample durations, accumulated totals,
 * and indexing information.
 *
 * @param window Pulse window to reset.
 */
static void reset_pulse_window(wheel_pulse_window_t *window)
{
	uint8_t index;

	window->total_pulses = 0U;
	window->total_time_ms = 0U;
	window->next_index = 0U;
	window->sample_count = 0U;

	for (index = 0U; index < WHEEL_WINDOW_SAMPLES; index++) {
		window->pulse_counts[index] = 0U;
		window->sample_times_ms[index] = 0U;
	}
}

/**
 * @brief Add one pulse-count sample to a rolling wheel pulse window.
 *
 * Once the window is full, the oldest pulse count and sample duration are
 * removed before the newest values are added.
 *
 * @param window Pulse window to update.
 * @param pulse_count Pulses counted during the latest sample.
 * @param elapsed_ms Duration of the latest sample in milliseconds.
 */
static void update_pulse_window(wheel_pulse_window_t *window,
				uint16_t pulse_count,
				uint32_t elapsed_ms)
{
	if (window->sample_count >= WHEEL_WINDOW_SAMPLES) {
		// Remove the sample that is about to be replaced.
		window->total_pulses -=
			(uint32_t)window->pulse_counts[window->next_index];

		window->total_time_ms -=
			window->sample_times_ms[window->next_index];
	} else {
		window->sample_count++;
	}

	// Store and accumulate the newest sample.
	window->pulse_counts[window->next_index] = pulse_count;
	window->sample_times_ms[window->next_index] = elapsed_ms;

	window->total_pulses += (uint32_t)pulse_count;
	window->total_time_ms += elapsed_ms;

	window->next_index++;

	if (window->next_index >= WHEEL_WINDOW_SAMPLES) {
		window->next_index = 0U;
	}
}

/**
 * @brief Calculate wheel RPM and MPH from pulse-count data.
 *
 * @param pulse_count Total pulses counted over the measurement window.
 * @param elapsed_ms Total duration of the measurement window.
 * @param rpm Destination for the calculated wheel speed in RPM.
 * @param mph Destination for the calculated vehicle speed in MPH.
 */
static void calculate_wheel_speed(uint32_t pulse_count,
				  uint32_t elapsed_ms,
				  float *rpm,
				  float *mph)
{
	float frequency_hz;

	if ((elapsed_ms == 0U) || (pulse_count == 0U)) {
		return;
	}

	frequency_hz =
		((float)pulse_count * 1000.0f) / (float)elapsed_ms;

	*rpm = (frequency_hz * 60.0f) / PULSES_PER_ROTATION;
	*mph = *rpm * WHEEL_CIRCUMFERENCE_M * RPM_TO_MPH;
}

/**
 * @brief Process one wheel's latest pulse-count sample.
 *
 * Updates the rolling pulse window, calculates the current wheel speed,
 * and sets the result to zero after the pulse timeout expires.
 *
 * @param window Pulse window associated with the wheel.
 * @param pulse_count Pulses counted during the latest sample.
 * @param elapsed_ms Duration of the latest sample.
 * @param current_tick Current ThreadX tick.
 * @param last_pulse_tick Tick at which a pulse was last detected.
 * @param rpm Destination for the wheel speed in RPM.
 * @param mph Destination for the wheel speed in MPH.
 */
static void process_wheel_sample(wheel_pulse_window_t *window,
				 uint16_t pulse_count,
				 uint32_t elapsed_ms,
				 uint32_t current_tick,
				 uint32_t *last_pulse_tick,
				 float *rpm,
				 float *mph)
{
	update_pulse_window(window, pulse_count, elapsed_ms);

	if (pulse_count > 0U) {
		*last_pulse_tick = current_tick;
	}

	if (window->total_pulses > 0U) {
		calculate_wheel_speed(window->total_pulses,
				      window->total_time_ms,
				      rpm,
				      mph);
	}

	if (TICKS_TO_MS(current_tick - *last_pulse_tick) >=
	    WHEEL_ZERO_TIMEOUT_MS) {
		if ((*rpm > 0.0f) || (window->total_pulses > 0U)) {
			*rpm = 0.0f;
			*mph = 0.0f;

			// Discard old samples before the wheel starts again.
			reset_pulse_window(window);
		}
	}
}

void wheel_speed_init(TIM_HandleTypeDef *_htim_left,
		      TIM_HandleTypeDef *_htim_right)
{
	htim_left = _htim_left;
	htim_right = _htim_right;

	// Establish a deterministic timer-counter baseline.
	__HAL_TIM_SET_COUNTER(htim_left, 0U);
	__HAL_TIM_SET_COUNTER(htim_right, 0U);

	left_previous_count = 0U;
	right_previous_count = 0U;

	reset_pulse_window(&left_pulse_window);
	reset_pulse_window(&right_pulse_window);

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
	uint32_t current_tick;
	uint32_t elapsed_ms;
	uint16_t left_current_count;
	uint16_t right_current_count;
	uint16_t left_pulse_count;
	uint16_t right_pulse_count;

	current_tick = (uint32_t)tx_time_get();
	elapsed_ms = TICKS_TO_MS(current_tick - previous_sample_tick);

	if (elapsed_ms < WHEEL_SAMPLE_PERIOD_MS) {
		return;
	}

	// Read the continuously incrementing hardware counters.
	left_current_count =
		(uint16_t)__HAL_TIM_GET_COUNTER(htim_left);

	right_current_count =
		(uint16_t)__HAL_TIM_GET_COUNTER(htim_right);

	left_pulse_count =
		get_pulse_count(left_current_count,
				left_previous_count);

	right_pulse_count =
		get_pulse_count(right_current_count,
				right_previous_count);

	// Use the current values as the reference for the next sample.
	left_previous_count = left_current_count;
	right_previous_count = right_current_count;
	previous_sample_tick = current_tick;

	process_wheel_sample(&left_pulse_window,
			     left_pulse_count,
			     elapsed_ms,
			     current_tick,
			     &left_last_pulse_tick,
			     &wheel_speed_data.left_rpm,
			     &wheel_speed_data.left_mph);

	process_wheel_sample(&right_pulse_window,
			     right_pulse_count,
			     elapsed_ms,
			     current_tick,
			     &right_last_pulse_tick,
			     &wheel_speed_data.right_rpm,
			     &wheel_speed_data.right_mph);
}

wheel_speed_data_t wheel_speed_get_data(void)
{
	return wheel_speed_data;
}
