#ifndef __U_QUEUES_H
#define __U_QUEUES_H

#include "u_tx_queues.h"

/* Queue List */
extern queue_t can_incoming; // Incoming CAN Queue
extern queue_t can_outgoing; // Outgoing CAN Queue
// add more as necessary

/* API */

/**
 * @brief initializes system queues
 * 
 * @returns returns 0 on success, 1 on fail
 */
uint8_t queues_init(TX_BYTE_POOL *byte_pool); // Initializes all queues. Called from app_threadx.c

#endif /* u_queues.h */