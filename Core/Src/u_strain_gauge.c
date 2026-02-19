#include <stdint.h>
#include "u_adcs.h"
#include "u_strain_gauge.h"
#include "u_utils.h"

#define STRAIN_GAUGE1_ZERO_OFFSET   0.0f
#define STRAIN_GAUGE1_SCALE_FACTOR  1.0f

#define STRAIN_GAUGE2_ZERO_OFFSET   0.0f
#define STRAIN_GAUGE2_SCALE_FACTOR  1.0f

#define STRAIN_GAUGE3_ZERO_OFFSET   0.0f
#define STRAIN_GAUGE3_SCALE_FACTOR  1.0f

#define STRAIN_GAUGE4_ZERO_OFFSET   0.0f
#define STRAIN_GAUGE4_SCALE_FACTOR  1.0f

strain_gauge_data_t strain_gauge_get_data() {
    raw_strain_gauge_adc_t rawData = adc_get_strain_gauge_data();

    float strain1Volts = adc_to_voltage(rawData.data[STRAIN_GAUGE1]);
    float strain2Volts = adc_to_voltage(rawData.data[STRAIN_GAUGE2]);
    float strain3Volts = adc_to_voltage(rawData.data[STRAIN_GAUGE3]);
    float strain4Volts = adc_to_voltage(rawData.data[STRAIN_GAUGE4]);

    strain_gauge_data_t sg;
    sg.strain[STRAIN_GAUGE1] = adc_calibrate(strain1Volts, STRAIN_GAUGE1_ZERO_OFFSET, STRAIN_GAUGE1_SCALE_FACTOR);
    sg.strain[STRAIN_GAUGE2] = adc_calibrate(strain2Volts, STRAIN_GAUGE2_ZERO_OFFSET, STRAIN_GAUGE2_SCALE_FACTOR);
    sg.strain[STRAIN_GAUGE3] = adc_calibrate(strain3Volts, STRAIN_GAUGE3_ZERO_OFFSET, STRAIN_GAUGE3_SCALE_FACTOR);
    sg.strain[STRAIN_GAUGE4] = adc_calibrate(strain4Volts, STRAIN_GAUGE4_ZERO_OFFSET, STRAIN_GAUGE4_SCALE_FACTOR);

    return sg;
}