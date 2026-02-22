#ifndef __U_STRAIN_GAUGE_H
#define __U_STRAIN_GAUGE_H

#include "u_adcs.h"

/** Data from strain gauge ADCs */
typedef struct {
    float strain[NUM_STRAIN_GAUGES]; /** Calibrated and converted ADC data */
} strain_gauge_data_t;

/**
 * @brief Reads the adc strain gauges value and returns the processed data
 * @return Calibrated data from the strain gauges
 */
strain_gauge_data_t strain_gauge_get_data();

#endif /* u_strain_guages.h */