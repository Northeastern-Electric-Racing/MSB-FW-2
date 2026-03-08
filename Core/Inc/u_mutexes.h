#ifndef __U_MUTEX_H
#define __U_MUTEX_H

#include "u_tx_mutex.h"

/* Mutex List */
extern mutex_t temp_mutex;       // Temp mutex until actual mutexes are needed for this project. Should probably be removed after an actual mutex is created.
// add more as necessary...

/* API */

/**
 * @brief initializes system mutexes
 * 
 * @returns returns 0 on success, 1 on fail
 */
uint8_t mutexes_init(); // Initializes all mutexes set up in u_mutexes.c

#endif /* u_mutex.h */