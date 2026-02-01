#include "fdcan.h"
#include "u_tx_debug.h"

void inbox_can(can_msg_t *message) {
    switch(message->id) {
        default:
            PRINTLN_ERROR("Unknown CAN Message Recieved (Message ID: %ld).", message->id);
            break;
    }
}