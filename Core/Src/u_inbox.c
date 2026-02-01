#include "fdcan.h"

void inbox_can(can_msg_t *message) {
    switch(message->id) {
        default:
            PRINTLN_ERROR("Unknown CAN Message Recieved (Message ID: %ld).", message->id);
            break;
    }
}