#include <stdint.h>
#include "u_adcs.h"
#include "u_thermocouple.h"
#include "u_utils.h"
#include "u_can.h"
#include "u_queues.h"

static float _voltage_to_temp(float voltage) {
    return ((voltage - 1.25f) / 0.005f) + 25.0f;
}

thermocouple_data_t thermocouple_get_data() {
    raw_thermocouple_adc_t rawData = adc_get_thermocouple_data();
    float voltage = adc_to_voltage(rawData.data[THERMOCOUPLE1]);

    thermocouple_data_t convertedData;
    convertedData.temp[THERMOCOUPLE1] = _voltage_to_temp(voltage);
    
    return convertedData;
}

void send_thermocouple_data(thermocouple_data_t data) {
    struct __attribute__((__packed__)) {
		int32_t thermocouple1;
	} thermocouple_data;

    thermocouple_data.thermocouple1 = data.temp[THERMOCOUPLE1];

    can_msg_t can_message = {.id = THERMOCOUPLE_CAN_ID, .len = 4, .data = {0}};

    memcpy(can_message.data, &thermocouple_data, can_message.len);

    queue_send(&can_outgoing, &can_message, TX_NO_WAIT);
}