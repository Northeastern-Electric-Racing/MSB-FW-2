#ifndef __U_THERMOCOUPLE_H
#define __U_THERMOCOUPLE_H

#include "u_adcs.h"

/** Data from Thermocuple ADC */
typedef struct {
    float temp[NUM_THERMOCOUPLES]; /** Calibrated and converted ADC data */
} thermocouple_data_t;

/**
 * @brief Reads the adc thermocouple value and returns the processed data
 * @return Calibrated data from the thermocouple
 */
thermocouple_data_t thermocouple_get_data();

#endif /* u_threads.h */