#include "u_tx_debug.h"
#include "main.c"

typedef enum {
    ADC1_CHANNEL0 = 0,
    ADC1_CHANNEL6,
    ADC1_CHANNEL10,
    ADC1_CHANNEL12,
    ADC1_CHANNEL13,

    ADC1_SIZE,
} _adc1_t;
static volatile uint16_t _adc1_buffer[ADC1_SIZE];

typedef enum {
    ADC2_CHANNEL15 = 0,
    ADC2_CHANNEL2,
    ADC2_CHANNEL6,

    ADC2_SIZE,
} _adc2_t;
static volatile uint16_t _adc2_buffer[ADC2_SIZE];

int adc_init(void) {
    uint16_t status = HAL_ADC_Start_DMA(&hadc1, _adc1_buffer, ADC1_SIZE);
    if(status != HAL_OK) {
        PRINTLN_ERROR("Failed to start ADC DMA for ADC1 (Status: %d/%s).", status, hal_status_toString(status));
        return U_ERROR;
    }

    status = HAL_ADC_Start_DMA(&hadc2, _adc2_buffer, ADC2_SIZE);
    if(status != HAL_OK) {
        PRINTLN_ERROR("Failed to start ADC DMA for ADC2 (Status: %d/%s).", status, hal_status_toString(status));
        return U_ERROR;
    }

    return U_SUCCESS;
}