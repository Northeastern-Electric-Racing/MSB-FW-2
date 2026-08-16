#include <stdint.h>
#include "u_adcs.h"
#include "u_shock_pot.h"
#include "u_utils.h"

#define SHOCK_POT1_ZERO_OFFSET   0.0f
#define SHOCK_POT1_SCALE_FACTOR  1.0f
#define SHOCK_POT1_CALIBRATED_V  3.3 //MEASURE AND REPLACE THIS

#define SHOCK_POT2_ZERO_OFFSET   0.0f
#define SHOCK_POT2_SCALE_FACTOR  1.0f
#define SHOCK_POT2_CALIBRATED_V  3.3 //MEASURE AND REPLACE THIS

#define SHOCK_POT_LENGTH_IN_MM   0.0 //REPLACE THIS
#define SHOCK_POT_LENGTH_IN_INCHES   1.9685 //REPLACE THIS 

shock_pot_data_t shock_pot_get_data() {
    raw_shock_pot_adc_t rawData = adc_get_shock_pot_data();

    float shockPot1Volts = adc_to_voltage(rawData.data[SHOCK_POT1]);
    float shockPot2Volts = adc_to_voltage(rawData.data[SHOCK_POT2]);

    shock_pot_data_t sp;
    sp.raw_adc[SHOCK_POT1] = rawData.data[SHOCK_POT1];
    sp.raw_adc[SHOCK_POT2] = rawData.data[SHOCK_POT2];
    sp.position[SHOCK_POT1] = adc_calibrate(shockPot1Volts, SHOCK_POT1_ZERO_OFFSET, SHOCK_POT1_SCALE_FACTOR);
    sp.position[SHOCK_POT2] = adc_calibrate(shockPot2Volts, SHOCK_POT2_ZERO_OFFSET, SHOCK_POT2_SCALE_FACTOR);

    sp.inch_travel[SHOCK_POT1] = (SHOCK_POT1_CALIBRATED_V - sp.position[SHOCK_POT1]) * (SHOCK_POT_LENGTH_IN_INCHES / 3.3f);
    sp.inch_travel[SHOCK_POT2] = (SHOCK_POT2_CALIBRATED_V - sp.position[SHOCK_POT2]) * (SHOCK_POT_LENGTH_IN_INCHES / 3.3f);

    return sp;
}


// shock_pot_data_t shock_pot_data_to_inches(shock_pot_data_t data) {
//     shock_pot_data_t sp;
//     sp.position[SHOCK_POT1] = data.position[SHOCK_POT1];
//     sp.position[SHOCK_POT2] = data.position[SHOCK_POT2];

//     sp.inch_travel[SHOCK_POT1] = SHOCK_POT1_CALIBRATED_V - ((sp.position[SHOCK_POT1]) * ((SHOCK_POT1_CALIBRATED_V)/(1.9685)));
//     sp.inch_travel[SHOCK_POT2] = SHOCK_POT2_CALIBRATED_V - ((sp.position[SHOCK_POT2]) * ((SHOCK_POT2_CALIBRATED_V)/(1.9685)));

//     return sp;
// }
