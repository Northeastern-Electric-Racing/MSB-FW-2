#include <stdint.h>
#include "u_adcs.h"
#include "u_load_cell.h"
#include "u_utils.h"

#define LOAD_CELL1_ZERO_OFFSET   0.0f
#define LOAD_CELL1_SCALE_FACTOR  1.0f

#define LOAD_CELL2_ZERO_OFFSET   0.0f
#define LOAD_CELL2_SCALE_FACTOR  1.0f

load_cell_data_t load_cell1_get_data() {
    raw_load_cell_adc_t rawData = adc_get_load_cell1_data();

    float loadCell1Volts = adc_to_voltage(rawData.data[LOAD_CELL1]);

    load_cell_data_t lc;
    lc.force[LOAD_CELL1] = adc_calibrate(loadCell1Volts, LOAD_CELL1_ZERO_OFFSET, LOAD_CELL1_SCALE_FACTOR);

    return lc;
}

load_cell_data_t load_cell2_get_data() {
    raw_load_cell_adc_t rawData = adc_get_load_cell2_data();

    float loadCell2Volts = adc_to_voltage(rawData.data[LOAD_CELL2]);

    load_cell_data_t lc;
    lc.force[LOAD_CELL1] = adc_calibrate(loadCell2Volts, LOAD_CELL1_ZERO_OFFSET, LOAD_CELL1_SCALE_FACTOR);

    return lc;
}