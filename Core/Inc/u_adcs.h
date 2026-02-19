#ifndef __U_ADCS_H
#define __U_ADCS_H

typedef enum {
    THERMOCOUPLE1 = 0,

    NUM_THERMOCOUPLES,
} thermocouple_t;

typedef enum {
    STRAIN_GAUGE1 = 0,
    STRAIN_GAUGE2,
    STRAIN_GAUGE3,
    STRAIN_GAUGE4,

    NUM_STRAIN_GAUGES,
} strain_gauge_t;

typedef enum {
    SHOCK_POT1 = 0,
    SHOCK_POT2,

    NUM_SHOCK_POTS,
} shock_pot_t;

typedef enum {
    STEERING_ANGLE1 = 0,

    NUM_STEERING_ANGLES,
} steering_angle_t;

typedef enum {
    LOAD_CELL1 = 0,
    LOAD_CELL2,

    NUM_LOAD_CELLS,
} load_cell_t;

typedef struct { 
    uint16_t data[NUM_THERMOCOUPLES]; 
} raw_thermocouple_adc_t;
typedef struct { 
    uint16_t data[NUM_STRAIN_GAUGES]; 
} raw_strain_gauge_adc_t;

typedef struct { 
    uint16_t data[NUM_SHOCK_POTS]; 
} raw_shock_pot_adc_t;

typedef struct { 
    uint16_t data[NUM_STEERING_ANGLES]; 
} raw_steering_angle_adc_t;

typedef struct { 
    uint16_t data[NUM_LOAD_CELLS]; 
} raw_load_cell_adc_t;

typedef struct { 
    uint16_t data;
} raw_misc_adc_t;

int adc_init();

typedef enum { 
    HIGH, 
    LOW 
} mux_state_t;

int adc_switchMuxStates(mux_state_t state);

raw_thermocouple_adc_t adc_get_thermocouple_data();
raw_strain_gauge_adc_t adc_get_strain_gauge_data();
raw_shock_pot_adc_t adc_get_shock_pot_data();
raw_steering_angle_adc_t adc_get_steering_angle_data();
raw_load_cell_adc_t adc_get_load_cell1_data();
raw_load_cell_adc_t adc_get_load_cell2_data();
raw_misc_adc_t adc_get_misc_adc1_data();
raw_misc_adc_t adc_get_misc_adc2_data();
raw_misc_adc_t adc_get_misc_adc3_data();

#endif /* u_adcs.h */