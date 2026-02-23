#include <stdint.h>
#include "u_adcs.h"
#include "u_shock_pot.h"
#include "u_utils.h"
#include "u_can.h"
#include "u_queues.h"

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

void send_shock_pot_data(shock_pot_data_t data) {
    struct __attribute__((__packed__)) {
		int32_t shock_pot1;
        int32_t shock_pot2;
	} shock_pot_data;

    shock_pot_data.shock_pot1 = data.position[SHOCK_POT1];
    shock_pot_data.shock_pot2 = data.position[SHOCK_POT2];

    can_msg_t can_message = {.id = SHOCK_POTS_CAN_ID, .len = 8, .data = {0}};

    memcpy(can_message.data, &shock_pot_data, can_message.len);

    queue_send(&can_outgoing, &can_message, TX_NO_WAIT);
}