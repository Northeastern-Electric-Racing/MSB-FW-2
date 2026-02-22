#include <stdint.h>
#include "u_utils.h"

int16_t float_to_int16(float value) {
    if (value > 32767.0f) {
        return 32767;
    }

    if (value < -32768.0f) {
        return -32768;
    }

    return (int16_t) value;
}

float adc_to_voltage(uint16_t rawData) {
    return (rawData * MAX_VOLTS) / MAX_ADC_VAL_12b;
}

float adc_calibrate(float volts, float zero_offset, float scale_factor) {
    return (volts - zero_offset) * scale_factor;
}