#ifndef __U_MISC_ADC_H
#define __U_MISC_ADC_H

#include "u_adcs.h"

typedef struct {
    float data; 
} misc_adc_data_t;

misc_adc_data_t misc_adc1_get_data();
misc_adc_data_t misc_adc2_get_data();
misc_adc_data_t misc_adc3_get_data();

#endif /* u_misc_adc.h */