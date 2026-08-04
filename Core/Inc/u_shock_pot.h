#ifndef __U_SHOCK_POT_H
#define __U_SHOCK_POT_H

#include "u_adcs.h"

/** ADC data from Shock Pot ADCs */
typedef struct {
    float position[NUM_SHOCK_POTS]; /** Calibrated and converted ADC data */
    float inch_travel[NUM_SHOCK_POTS];
} shock_pot_data_t;

/**
 * @brief Reads the adc shock pots value and returns the processed data
 * @return Calibrated data from the shock pots
 */
shock_pot_data_t shock_pot_get_data();

/**
* @brief Takes in shock_pot_data_t data and updates the inch_travel value to represent the inches travelled of each of the shock pots
* @param data Must be of shock_pot_data_t type
* @return Calibrated data from the shock pots INCLUDING INCHES TRAVELED WITHIN inch_travel[]
*/
// shock_pot_data_t shock_pot_data_to_inches(shock_pot_data_t data);

/**
 * @brief Sends the shock pot data over CAN
 */
void send_shock_pot_data(shock_pot_data_t data);

#endif /* u_shock_pot.h */