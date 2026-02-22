#ifndef __U_ADCS_H
#define __U_ADCS_H

/** Thermocouples connected the MSB */
typedef enum {
    THERMOCOUPLE1 = 0,

    NUM_THERMOCOUPLES,
} thermocouple_t;

/** Strain Gauges connected the MSB */
typedef enum {
    STRAIN_GAUGE1 = 0,
    STRAIN_GAUGE2,
    STRAIN_GAUGE3,
    STRAIN_GAUGE4,

    NUM_STRAIN_GAUGES,
} strain_gauge_t;

/** Shock Pots connected the MSB */
typedef enum {
    SHOCK_POT1 = 0,
    SHOCK_POT2,

    NUM_SHOCK_POTS,
} shock_pot_t;

/** Steering Angle connected the MSB */
typedef enum {
    STEERING_ANGLE1 = 0,

    NUM_STEERING_ANGLES,
} steering_angle_t;

/** Load Cells connected the MSB */
typedef enum {
    LOAD_CELL1 = 0,
    LOAD_CELL2,

    NUM_LOAD_CELLS,
} load_cell_t;

/** Struct storing raw data from thermocouple */
typedef struct { 
    uint16_t data[NUM_THERMOCOUPLES]; /** Raw thermocouple data */
} raw_thermocouple_adc_t;

/** Struct storing raw data from strain gauges */
typedef struct { 
    uint16_t data[NUM_STRAIN_GAUGES]; /** Raw strain gauge data */
} raw_strain_gauge_adc_t;

/** Struct storing raw data from shock pots */
typedef struct { 
    uint16_t data[NUM_SHOCK_POTS]; /** Raw shock pot data */
} raw_shock_pot_adc_t;

/** Struct storing raw data from steering angle */
typedef struct { 
    uint16_t data[NUM_STEERING_ANGLES]; /** Raw steering angle data */
} raw_steering_angle_adc_t;

/** Struct storing raw data from load cell */
typedef struct { 
    uint16_t data[NUM_LOAD_CELLS]; /** Raw load cell data */
} raw_load_cell_adc_t;

/** Struct storing raw data from miscellaneous adc */
typedef struct { 
    uint16_t data; /** Raw miscellaneous adc data */
} raw_misc_adc_t;

/**
 * @brief Initializes DMA for ADC 1 and ADC 2
 * @return Returns U_SUCCESS on success; U_ERROR on fail
 */
int adc_init();

/** Multiplexer States */
typedef enum { 
    HIGH,
    LOW 
} mux_state_t;

/**
 * @brief Switches the two multiplexer states to the given parameter
 * @param state The state to switch the multiplexers to
 * @return Returns U_SUCCESS on success
 */
int adc_switchMuxStates(mux_state_t state);

/**
 * @brief Reads raw ADC reading from Thermocouple
 * @return Raw ADC reading from Thermocouple
 */
raw_thermocouple_adc_t adc_get_thermocouple_data();

/**
 * @brief Reads raw ADC reading from Strain Gauges
 * @return Raw ADC reading from Strain Gauges
 */
raw_strain_gauge_adc_t adc_get_strain_gauge_data();

/**
 * @brief Reads raw ADC reading from Shock Pots
 * @return Raw ADC reading from Shock Pots
 */
raw_shock_pot_adc_t adc_get_shock_pot_data();

/**
 * @brief Reads raw ADC reading from Steering Angle
 * @return Raw ADC reading from Steering Angle
 */
raw_steering_angle_adc_t adc_get_steering_angle_data();

/**
 * @brief Reads raw ADC reading from Load Cell 1
 * @return Raw ADC reading from Load Cell 1
 */
raw_load_cell_adc_t adc_get_load_cell1_data();

/**
 * @brief Reads raw ADC reading from Load Cell 2
 * @return Raw ADC reading from Load Cell 2
 */
raw_load_cell_adc_t adc_get_load_cell2_data();

/**
 * @brief Reads raw ADC reading from Miscellaneous ADC 1
 * @return Raw ADC reading from Load Miscellaneous ADC 1
 */
raw_misc_adc_t adc_get_misc_adc1_data();

/**
 * @brief Reads raw ADC reading from Miscellaneous ADC 2
 * @return Raw ADC reading from Load Miscellaneous ADC 2
 */
raw_misc_adc_t adc_get_misc_adc2_data();

/**
 * @brief Reads raw ADC reading from Miscellaneous ADC 3
 * @return Raw ADC reading from Load Miscellaneous ADC 3
 */
raw_misc_adc_t adc_get_misc_adc3_data();

#endif /* u_adcs.h */