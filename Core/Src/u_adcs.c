#include "u_tx_debug.h"
#include "u_adcs.h"
#include "main.h"

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

int adc_switchMuxStates(mux_state_t state) {
    if (state == HIGH) {
        HAL_GPIO_WritePin(MUX1_SEL1_GPIO_Port, MUX1_SEL1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX1_SEL2_GPIO_Port, MUX1_SEL2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX1_SEL3_GPIO_Port, MUX1_SEL3_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX1_SEL4_GPIO_Port, MUX1_SEL4_Pin, GPIO_PIN_SET);

        HAL_GPIO_WritePin(MUX2_SEL1_GPIO_Port, MUX2_SEL1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX2_SEL2_GPIO_Port, MUX2_SEL2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX2_SEL3_GPIO_Port, MUX2_SEL3_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX2_SEL4_GPIO_Port, MUX2_SEL4_Pin, GPIO_PIN_SET);

        tx_thread_sleep(1);

        PRINTLN_INFO("Switched mux_state to HIGH.");
    }
    else {
        HAL_GPIO_WritePin(MUX1_SEL1_GPIO_Port, MUX1_SEL1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX1_SEL2_GPIO_Port, MUX1_SEL2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX1_SEL3_GPIO_Port, MUX1_SEL3_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX1_SEL4_GPIO_Port, MUX1_SEL4_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(MUX2_SEL1_GPIO_Port, MUX2_SEL1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX2_SEL2_GPIO_Port, MUX2_SEL2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX2_SEL3_GPIO_Port, MUX2_SEL3_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX2_SEL4_GPIO_Port, MUX2_SEL4_Pin, GPIO_PIN_RESET);

        tx_thread_sleep(1);

        PRINTLN_INFO("Switched mux_state to LOW.");
    }

    return U_SUCCESS;
}

int adc_init() {
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

raw_thermocouple_adc_t adc_get_thermocouple_data() {
    raw_thermocouple_adc_t tc;

    tc.data[THERMOCOUPLE1] = _adc2_buffer[ADC2_CHANNEL15];

    return tc;
}

raw_strain_gauge_adc_t adc_get_strain_gauge_data() {
    raw_strain_gauge_adc_t sg;

    sg.data[STRAIN_GAUGE1] = _adc1_buffer[ADC1_CHANNEL10];
    sg.data[STRAIN_GAUGE2] = _adc1_buffer[ADC1_CHANNEL12];
    sg.data[STRAIN_GAUGE3] = _adc1_buffer[ADC1_CHANNEL13];
    sg.data[STRAIN_GAUGE4] = _adc1_buffer[ADC1_CHANNEL0];

    return sg;
}

raw_shock_pot_adc_t adc_get_shock_pot_data() {
    raw_shock_pot_adc_t sp;

    sp.data[SHOCK_POT1] = _adc1_buffer[ADC1_CHANNEL10];
    sp.data[SHOCK_POT2] = _adc1_buffer[ADC1_CHANNEL12];

    return sp;
}

raw_steering_angle_adc_t adc_get_steering_angle_data() {
    raw_steering_angle_adc_t sa;

    sa.data[STEERING_ANGLE1] = _adc1_buffer[ADC1_CHANNEL13];

    return sa;
}

raw_load_cell_adc_t adc_get_load_cell1_data() {
    raw_load_cell_adc_t lc;

    lc.data[LOAD_CELL1] = _adc2_buffer[ADC2_CHANNEL15];
    lc.data[LOAD_CELL2] = 0;

    return lc;
}

raw_load_cell_adc_t adc_get_load_cell2_data() {
    raw_load_cell_adc_t lc;

    lc.data[LOAD_CELL1] = 0;
    lc.data[LOAD_CELL2] = _adc2_buffer[ADC2_CHANNEL2];

    return lc;
}

raw_misc_adc_t adc_get_misc_adc1_data() {
    raw_misc_adc_t ms;
    ms.data = _adc1_buffer[ADC1_CHANNEL0];
    return ms;
}

raw_misc_adc_t adc_get_misc_adc2_data() {
    raw_misc_adc_t ms;
    ms.data = _adc2_buffer[ADC2_CHANNEL6];
    return ms;
}

raw_misc_adc_t adc_get_misc_adc3_data() {
    raw_misc_adc_t ms;
    ms.data = _adc2_buffer[ADC2_CHANNEL2];
    return ms;
}