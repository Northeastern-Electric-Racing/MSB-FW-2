#include "u_mutexes.h"

/* Temp Mutex (placeholder until an actual mutex is needed in this project.) */
mutex_t temp_mutex = {
    .name = "Temp Mutex",           /* Name of the mutex. */
    .priority_inherit = TX_INHERIT /* Priority inheritance setting. */
};

/* Initializes all ThreadX mutexes. 
*  Calls to create_mutex() should go in here
*/
uint8_t mutexes_init() {
    /* Create Mutexes. */
    CATCH_ERROR(create_mutex(&temp_mutex), U_SUCCESS);       // Create Temp Mutex.
    // add more as necessary.

    DEBUG_PRINTLN("Ran mutexes_init().");
    return U_SUCCESS;
}