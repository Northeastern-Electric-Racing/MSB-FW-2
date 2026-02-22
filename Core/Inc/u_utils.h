#ifndef __U_UTILS_H
#define __U_UTILS_H

#define MAX_ADC_VAL_12b 4096.0f
#define MAX_VOLTS       3.3f

/** Location of MSB on the vehicle */
typedef enum {
	DEVICE_FRONT,
	DEVICE_BACK,
} device_loc_t;

/**
 * Converts a float to a 16 bit integer.
 * 
 * @param value The float value to be converted
 * 
 * @return The converted 16 bit integer
 */
int16_t float_to_int16(float value);

/**
 * @brief Converts raw adc value to volts
 * 
 * @param rawData The raw adc value
 * 
 * @return The raw adc value converted to volts as a float
 */
float adc_to_voltage(uint16_t rawData);

/**
 * @brief Returns a calibrated value based on the given adc volts and calibration values
 * 
 * @param volts The volts from the adc
 * @param zero_offset The offset to zero the adc value
 * @param scale_factor The scale factor in calibrating
 * 
 * @return A calibrated adc value
 */
float adc_calibrate(float volts, float zero_offset, float scale_factor);

#endif /* u_utils.h */