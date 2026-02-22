#ifndef __U_SHOCK_POT_H
#define __U_SHOCK_POT_H

#include "u_adcs.h"

/** ADC data from Shock Pot ADCs */
typedef struct {
    float position[NUM_SHOCK_POTS]; /** Calibrated and converted ADC data */
} shock_pot_data_t;

/**
 * @brief Reads the adc shock pots value and returns the processed data
 * @return Calibrated data from the shock pots
 */
shock_pot_data_t shock_pot_get_data();

#endif /* u_shock_pot.h */