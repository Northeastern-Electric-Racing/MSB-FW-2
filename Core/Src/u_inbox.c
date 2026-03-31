#include "fdcan.h"
#include "u_tx_debug.h"
#include "u_can.h"
#include "u_sensors.h"
#include "main.h"

void inbox_can(can_msg_t *message) {
    switch(message->id) {
        case STEERING_ANGLE_CAN_ID:
            if (device_loc == DEVICE_BACK) {
                calibrate();
            }
            break;
        default:
            PRINTLN_ERROR("Unknown CAN Message Recieved (Message ID: %ld).", message->id);
            break;
    }
}