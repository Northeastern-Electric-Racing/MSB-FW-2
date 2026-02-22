#ifndef __U_THREADS_H
#define __U_THREADS_H

#include "tx_api.h"
#include "u_tx_threads.h"

/**
 * @brief creates system threads
 * 
 * @returns returns 0 on success, 1 on fail
 */
uint8_t threads_init(TX_BYTE_POOL *byte_pool);

/* Thread Functions */

void default_thread(ULONG thread_input);
void can_incoming_thread(ULONG thread_input);
void can_outgoing_thread(ULONG thread_input);
void sensors_thread(ULONG thread_input);

#endif /* u_threads.h */