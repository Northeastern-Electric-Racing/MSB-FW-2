#include <stdint.h>
#include "u_adcs.h"
#include "u_shock_pot.h"
#include "u_utils.h"

#define SHOCK_POT1_ZERO_OFFSET   0.0f
#define SHOCK_POT1_SCALE_FACTOR  1.0f

#define SHOCK_POT2_ZERO_OFFSET   0.0f
#define SHOCK_POT2_SCALE_FACTOR  1.0f

shock_pot_data_t shock_pot_get_data() {
    raw_shock_pot_adc_t rawData = adc_get_shock_pot_data();

    float shockPot1Volts = adc_to_voltage(rawData.data[SHOCK_POT1]);
    float shockPot2Volts = adc_to_voltage(rawData.data[SHOCK_POT2]);

    shock_pot_data_t sp;
    sp.position[SHOCK_POT1] = adc_calibrate(shockPot1Volts, SHOCK_POT1_ZERO_OFFSET, SHOCK_POT1_SCALE_FACTOR);
    sp.position[SHOCK_POT2] = adc_calibrate(shockPot2Volts, SHOCK_POT2_ZERO_OFFSET, SHOCK_POT2_SCALE_FACTOR);

    return sp;
}