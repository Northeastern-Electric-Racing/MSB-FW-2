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

/* Multiplexer buffer. */
/* Several ADC channels are multiplexed: a single channel reads one sensor when the muxes are driven HIGH
 * and a different sensor when driven LOW. adc_switchMuxState() latches each state's readings into this
 * buffer so the data getters always return the correct value regardless of the current pin state. */
typedef enum {
    /* HIGH-state readings (mux SEL lines SET). */
    MUX_STRAIN_GAUGE1,   // ADC1_CHANNEL10
    MUX_STRAIN_GAUGE2,   // ADC1_CHANNEL12
    MUX_STRAIN_GAUGE3,   // ADC1_CHANNEL13
    MUX_STRAIN_GAUGE4,   // ADC1_CHANNEL0
    MUX_THERMOCOUPLE1,   // ADC2_CHANNEL15
    MUX_LOAD_CELL2,      // ADC2_CHANNEL2
    MUX_MISC_ADC2,       // ADC1_CHANNEL6 (HIGH)

    /* LOW-state readings (mux SEL lines RESET). */
    MUX_SHOCK_POT1,      // ADC1_CHANNEL10
    MUX_SHOCK_POT2,      // ADC1_CHANNEL12
    MUX_STEERING_ANGLE1, // ADC1_CHANNEL13
    MUX_MISC_ADC1,       // ADC2_CHANNEL6 (LOW)
    MUX_LOAD_CELL1,      // ADC2_CHANNEL15
    MUX_MISC_ADC3,       // ADC1_CHANNEL6 (LOW)

    /* Total number of indexes for the multiplexer buffer. */
    MUX_SIZE,
} _mux_t;
static volatile uint16_t _mux_buffer[MUX_SIZE] = { 0 };

/* Manages the muxes and updates the mux buffer. */
/* Latches the readings for the current state into the mux buffer, then switches to the other state and
 * lets it settle. The state toggles internally on each call, so the caller just calls this repeatedly. */
static mux_state_t mux_state_debug = LOW;
int adc_switchMuxState(void) {
    static mux_state_t mux_state = LOW;

    if (mux_state == LOW) {
        /* Mux is currently LOW, so latch the LOW readings. */
        _mux_buffer[MUX_SHOCK_POT1]      = _adc1_buffer[ADC1_CHANNEL10];
        _mux_buffer[MUX_SHOCK_POT2]      = _adc1_buffer[ADC1_CHANNEL12];
        _mux_buffer[MUX_STEERING_ANGLE1] = _adc1_buffer[ADC1_CHANNEL13];
        _mux_buffer[MUX_MISC_ADC1]       = _adc2_buffer[ADC2_CHANNEL6];
        _mux_buffer[MUX_LOAD_CELL1]      = _adc2_buffer[ADC2_CHANNEL15];
        _mux_buffer[MUX_MISC_ADC2]       = _adc1_buffer[ADC1_CHANNEL6];

        /* Mux is currently LOW, so switch to HIGH. */
        HAL_GPIO_WritePin(MUX1_SEL1_GPIO_Port, MUX1_SEL1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX1_SEL2_GPIO_Port, MUX1_SEL2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX1_SEL3_GPIO_Port, MUX1_SEL3_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX1_SEL4_GPIO_Port, MUX1_SEL4_Pin, GPIO_PIN_SET);

        HAL_GPIO_WritePin(MUX2_SEL1_GPIO_Port, MUX2_SEL1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX2_SEL2_GPIO_Port, MUX2_SEL2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX2_SEL3_GPIO_Port, MUX2_SEL3_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MUX2_SEL4_GPIO_Port, MUX2_SEL4_Pin, GPIO_PIN_SET);

        tx_thread_sleep(1000); // Sleep for 10 ticks so the mux settles.
        mux_state = HIGH;    // Update the mux state.

        PRINTLN_INFO("Switched mux_state to HIGH.");
    }
    else {
        /* Mux is currently HIGH, so latch the HIGH readings. */
        _mux_buffer[MUX_STRAIN_GAUGE1] = _adc1_buffer[ADC1_CHANNEL10];
        _mux_buffer[MUX_STRAIN_GAUGE2] = _adc1_buffer[ADC1_CHANNEL12];
        _mux_buffer[MUX_STRAIN_GAUGE3] = _adc1_buffer[ADC1_CHANNEL13];
        _mux_buffer[MUX_STRAIN_GAUGE4] = _adc1_buffer[ADC1_CHANNEL0];
        _mux_buffer[MUX_THERMOCOUPLE1] = _adc2_buffer[ADC2_CHANNEL15];
        _mux_buffer[MUX_LOAD_CELL2]    = _adc2_buffer[ADC2_CHANNEL2];
        _mux_buffer[MUX_MISC_ADC3]     = _adc1_buffer[ADC1_CHANNEL6];

        /* Mux is currently HIGH,   so switch to LOW. */
        HAL_GPIO_WritePin(MUX1_SEL1_GPIO_Port, MUX1_SEL1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX1_SEL2_GPIO_Port, MUX1_SEL2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX1_SEL3_GPIO_Port, MUX1_SEL3_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX1_SEL4_GPIO_Port, MUX1_SEL4_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(MUX2_SEL1_GPIO_Port, MUX2_SEL1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX2_SEL2_GPIO_Port, MUX2_SEL2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX2_SEL3_GPIO_Port, MUX2_SEL3_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MUX2_SEL4_GPIO_Port, MUX2_SEL4_Pin, GPIO_PIN_RESET);

        tx_thread_sleep(1000); // Sleep for 10 ticks so the mux settles.
        mux_state = LOW;     // Update the mux state.

        PRINTLN_INFO("Switched mux_state to LOW.");
    }
    mux_state_debug = mux_state;

    PRINTLN_INFO("Ran adc_switchMuxState().");

    return U_SUCCESS;
}

int adc_init() {
    uint16_t status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)_adc1_buffer, ADC1_SIZE);
    if(status != HAL_OK) {
        PRINTLN_ERROR("Failed to start ADC DMA for ADC1 (Status: %d/%s).", status, hal_status_toString(status));
        return U_ERROR;
    }

    status = HAL_ADC_Start_DMA(&hadc2, (uint32_t *)_adc2_buffer, ADC2_SIZE);
    if(status != HAL_OK) {
        PRINTLN_ERROR("Failed to start ADC DMA for ADC2 (Status: %d/%s).", status, hal_status_toString(status));
        return U_ERROR;
    }


    PRINTLN_INFO("Successfully initialized ADCs and started DMA transfers.");
    return U_SUCCESS;
}

raw_thermocouple_adc_t adc_get_thermocouple_data() {
    raw_thermocouple_adc_t tc;

    tc.data[THERMOCOUPLE1] = _mux_buffer[MUX_THERMOCOUPLE1];

    return tc;
}

raw_strain_gauge_adc_t adc_get_strain_gauge_data() {
    raw_strain_gauge_adc_t sg;

    sg.data[STRAIN_GAUGE1] = _mux_buffer[MUX_STRAIN_GAUGE1];
    sg.data[STRAIN_GAUGE2] = _mux_buffer[MUX_STRAIN_GAUGE2];
    sg.data[STRAIN_GAUGE3] = _mux_buffer[MUX_STRAIN_GAUGE3];
    sg.data[STRAIN_GAUGE4] = _mux_buffer[MUX_STRAIN_GAUGE4];

    return sg;
}

raw_shock_pot_adc_t adc_get_shock_pot_data() {
    raw_shock_pot_adc_t sp;

    sp.data[SHOCK_POT1] = _mux_buffer[MUX_SHOCK_POT1];
    sp.data[SHOCK_POT2] = _mux_buffer[MUX_SHOCK_POT2];

    return sp;
}

raw_steering_angle_adc_t adc_get_steering_angle_data() {
    raw_steering_angle_adc_t sa;

    sa.data[STEERING_ANGLE1] = _mux_buffer[MUX_STEERING_ANGLE1];

    return sa;
}

raw_load_cell_adc_t adc_get_load_cell1_data() {
    raw_load_cell_adc_t lc;

    lc.data[LOAD_CELL1] = _mux_buffer[MUX_LOAD_CELL1];
    lc.data[LOAD_CELL2] = 0;

    return lc;
}

raw_load_cell_adc_t adc_get_load_cell2_data() {
    raw_load_cell_adc_t lc;

    lc.data[LOAD_CELL1] = 0;
    lc.data[LOAD_CELL2] = _mux_buffer[MUX_LOAD_CELL2];

    return lc;
}

raw_misc_adc_t adc_get_misc_adc1_data() {
    raw_misc_adc_t ms;
    ms.data = _mux_buffer[MUX_MISC_ADC1];
    return ms;
}

raw_misc_adc_t adc_get_misc_adc2_data() {
    raw_misc_adc_t ms;
    ms.data = _mux_buffer[MUX_MISC_ADC2];
    return ms;
}

raw_misc_adc_t adc_get_misc_adc3_data() {
    raw_misc_adc_t ms;
    ms.data = _mux_buffer[MUX_MISC_ADC3];
    return ms;
}