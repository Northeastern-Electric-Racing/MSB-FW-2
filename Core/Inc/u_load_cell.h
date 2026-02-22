#ifndef __U_LOAD_CELL_H
#define __U_LOAD_CELL_H

#include "u_adcs.h"

/** Data from Load Cell ADC */
typedef struct {
    float force[NUM_LOAD_CELLS]; /** Calibrated and converted ADC data */
} load_cell_data_t;

/**
 * @brief Reads the load cell 1 adc value and returns the processed data
 * @return Calibrated data from load cell 1
 */
load_cell_data_t load_cell1_get_data();

/**
 * @brief Reads the load cell 2 adc value and returns the processed data
 * @return Calibrated data from load cell 2
 */
load_cell_data_t load_cell2_get_data();

#endif /* u_load_cell.h */