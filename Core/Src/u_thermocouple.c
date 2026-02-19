#include <stdint.h>
#include "u_adcs.h"
#include "u_thermocouple.h"
#include "u_utils.h"

static float _voltage_to_temp(float voltage) {
    return ((voltage - 1.25f) / 0.005f) + 25.0f;
}

thermocouple_data_t thermocouple_get_data() {
    raw_thermocouple_adc_t rawData = adc_get_thermocouple_data();
    float voltage = adc_to_voltage(rawData.data[THERMOCOUPLE1]);

    thermocouple_data_t convertedData;
    convertedData.temp[THERMOCOUPLE1] = _voltage_to_temp(voltage);
    
    return convertedData;
}