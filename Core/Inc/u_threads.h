#pragma once

#include "tx_api.h"
#include "u_tx_threads.h"

/* Initializes all threads. Called from app_threadx.c */
uint8_t threads_init(TX_BYTE_POOL *byte_pool);

/* Thread Functions */
void default_thread(ULONG thread_input);