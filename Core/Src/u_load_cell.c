#include <stdint.h>
#include "u_adcs.h"
#include "u_load_cell.h"
#include "u_utils.h"
#include "u_can.h"
#include "u_queues.h"

#define LOAD_CELL1_ZERO_OFFSET   0.0f
#define LOAD_CELL1_SCALE_FACTOR  1.0f

#define LOAD_CELL2_ZERO_OFFSET   0.0f
#define LOAD_CELL2_SCALE_FACTOR  1.0f

load_cell_data_t load_cell1_get_data() {
    raw_load_cell_adc_t rawData = adc_get_load_cell1_data();

    float loadCell1Volts = adc_to_voltage(rawData.data[LOAD_CELL1]);

    load_cell_data_t lc;
    lc.force[LOAD_CELL1] = adc_calibrate(loadCell1Volts, LOAD_CELL1_ZERO_OFFSET, LOAD_CELL1_SCALE_FACTOR);

    return lc;
}

load_cell_data_t load_cell2_get_data() {
    raw_load_cell_adc_t rawData = adc_get_load_cell2_data();

    float loadCell2Volts = adc_to_voltage(rawData.data[LOAD_CELL2]);

    load_cell_data_t lc;
    lc.force[LOAD_CELL1] = adc_calibrate(loadCell2Volts, LOAD_CELL1_ZERO_OFFSET, LOAD_CELL1_SCALE_FACTOR);

    return lc;
}

void send_load_cell_data(load_cell_data_t lc1_data, load_cell_data_t lc2_data) {
    struct __attribute__((__packed__)) {
		int32_t load_cell1;
        int32_t load_cell2;
	} load_cell_data;

    load_cell_data.load_cell1 = lc1_data.force[LOAD_CELL1];
    load_cell_data.load_cell2 = lc2_data.force[LOAD_CELL2];

    can_msg_t can_message = {.id = LOAD_CELLS_CAN_ID, .len = 8, .data = {0}};

    memcpy(can_message.data, &load_cell_data, can_message.len);

    queue_send(&can_outgoing, &can_message, TX_NO_WAIT);
}