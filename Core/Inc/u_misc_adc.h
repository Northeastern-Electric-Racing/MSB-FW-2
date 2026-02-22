#ifndef __U_MISC_ADC_H
#define __U_MISC_ADC_H

#include "u_adcs.h"

/** Data from a miscellaneous adc */
typedef struct {
    float data; /** Calibrated and converted ADC data */
} misc_adc_data_t;

/**
 * @brief Reads the miscellaneous adc 1 value and returns the processed data
 * @return Calibrated data from miscellaneous adc 1
 */
misc_adc_data_t misc_adc1_get_data();

/**
 * @brief Reads the miscellaneous adc 2 value and returns the processed data
 * @return Calibrated data from miscellaneous adc 2
 */
misc_adc_data_t misc_adc2_get_data();

/**
 * @brief Reads the miscellaneous adc 3 value and returns the processed data
 * @return Calibrated data from miscellaneous adc 3
 */
misc_adc_data_t misc_adc3_get_data();

#endif /* u_misc_adc.h */