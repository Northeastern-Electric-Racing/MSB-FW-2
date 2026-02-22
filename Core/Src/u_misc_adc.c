#include <stdint.h>
#include "u_adcs.h"
#include "u_misc_adc.h"
#include "u_utils.h"

#define MISC_ADC1_ZERO_OFFSET   0.0f
#define MISC_ADC1_SCALE_FACTOR  1.0f

#define MISC_ADC2_ZERO_OFFSET   0.0f
#define MISC_ADC2_SCALE_FACTOR  1.0f

#define MISC_ADC3_ZERO_OFFSET   0.0f
#define MISC_ADC3_SCALE_FACTOR  1.0f

misc_adc_data_t misc_adc1_get_data() {
    raw_misc_adc_t rawData = adc_get_misc_adc1_data();

    float volts = adc_to_voltage(rawData.data);

    misc_adc_data_t ms;
    ms.data = adc_calibrate(volts, MISC_ADC1_ZERO_OFFSET, MISC_ADC1_SCALE_FACTOR);

    return ms;
}

misc_adc_data_t misc_adc2_get_data() {
    raw_misc_adc_t rawData = adc_get_misc_adc2_data();

    float volts = adc_to_voltage(rawData.data);

    misc_adc_data_t ms;
    ms.data = adc_calibrate(volts, MISC_ADC2_ZERO_OFFSET, MISC_ADC2_SCALE_FACTOR);

    return ms;
}

misc_adc_data_t misc_adc3_get_data() {
    raw_misc_adc_t rawData = adc_get_misc_adc3_data();

    float volts = adc_to_voltage(rawData.data);

    misc_adc_data_t ms;
    ms.data = adc_calibrate(volts, MISC_ADC3_ZERO_OFFSET, MISC_ADC3_SCALE_FACTOR);

    return ms;
}