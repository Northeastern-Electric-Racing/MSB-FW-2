#pragma once

#include "u_tx_mutex.h"

/* Mutex List */
extern mutex_t temp_mutex;       // Temp mutex until actual mutexes are needed for this project. Should probably be removed after an actual mutex is created.
// add more as necessary...

/* API */
uint8_t mutexes_init(); // Initializes all mutexes set up in u_mutexes.c