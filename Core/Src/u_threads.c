#include "u_threads.h"
#include "u_tx_debug.h"
#include "u_sensors.h"

/* Default Thread */
static thread_t _default_thread = {
        .name       = "Default Thread",  /* Name */
        .size       = 512,               /* Stack Size (in bytes) */
        .priority   = 9,                 /* Priority */
        .threshold  = 0,                 /* Preemption Threshold */
        .time_slice = TX_NO_TIME_SLICE,  /* Time Slice */
        .auto_start = TX_AUTO_START,     /* Auto Start */
        .sleep      = 50,                /* Sleep (in ticks) */
        .function   = default_thread     /* Thread Function */
    };
void default_thread(ULONG thread_input) {
    
    while(1) {

        // u_TODO - the watchdogs gotta be pet here probably

        /* Sleep Thread for specified number of ticks. */
        tx_thread_sleep(_default_thread.sleep);
    }
}

/* Sensors Thread */
static thread_t _sensors_thread = {
    .name       = "Sensors Thread",  /* Name */
    .size       = 512,               /* Stack Size (in bytes) */
    .priority   = 9,                 /* Priority */
    .threshold  = 0,                 /* Preemption Threshold */
    .time_slice = TX_NO_TIME_SLICE,  /* Time Slice */
    .auto_start = TX_AUTO_START,     /* Auto Start */
    .sleep      = 500,               /* Sleep (in ticks) */
    .function   = sensors_thread     /* Thread Function */
};
void sensors_thread(ULONG thread_input) {

    while(1) {
        read_sht30();
        tx_thread_sleep(_sensors_thread.sleep);
    }
}

/* Initializes all ThreadX threads. 
*  Calls to create_thread() should go in here
*/
uint8_t threads_init(TX_BYTE_POOL *byte_pool) {

    /* Create Threads */
    CATCH_ERROR(create_thread(byte_pool, &_default_thread), U_SUCCESS);      // Create Default thread.
    CATCH_ERROR(create_thread(byte_pool, &_sensors_thread), U_SUCCESS);      // Create Sensors` thread.

    PRINTLN_INFO("Ran threads_init().");
    return U_SUCCESS;
}