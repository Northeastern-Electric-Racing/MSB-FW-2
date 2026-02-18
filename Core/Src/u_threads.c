#include "u_threads.h"
#include "u_tx_debug.h"
#include "u_sensors.h"
#include "timer.h"
#include "tx_api.h"

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
static nertimer_t data_send_timer;
static thread_t _sensors_thread = {
    .name       = "Sensors Thread",  /* Name */
    .size       = 1024,              /* Stack Size (in bytes) */
    .priority   = 9,                 /* Priority */
    .threshold  = 0,                 /* Preemption Threshold */
    .time_slice = TX_NO_TIME_SLICE,  /* Time Slice */
    .auto_start = TX_AUTO_START,     /* Auto Start */
    .sleep      = 20,                /* Sleep (in ticks) */
    .function   = sensors_thread     /* Thread Function */
};
void sensors_thread(ULONG thread_input) {
    const uint16_t DATA_SEND_INTERVAL = 25 * _sensors_thread.sleep;
    start_timer(&data_send_timer, DATA_SEND_INTERVAL);

    while(1) {
        CATCH_ERROR(read_imu_and_magnometer(), U_SUCCESS);

        if (is_timer_expired(&data_send_timer)) {
            CATCH_ERROR(read_sht30(), U_SUCCESS);
            send_sht30_data();
            send_imu_and_magnometer_data();
            start_timer(&data_send_timer, DATA_SEND_INTERVAL);
        }

        tx_thread_sleep(_sensors_thread.sleep);
    }
}

/* Initializes all ThreadX threads. 
*  Calls to create_thread() should go in here
*/
uint8_t threads_init(TX_BYTE_POOL *byte_pool) {

    /* Create Threads */
    CATCH_ERROR(create_thread(byte_pool, &_default_thread), U_SUCCESS);      // Create Default thread.
    CATCH_ERROR(create_thread(byte_pool, &_sensors_thread), U_SUCCESS);      // Create Sensors thread.

    PRINTLN_INFO("Ran threads_init().");
    return U_SUCCESS;
}

/* Incoming Ethernet Thread. Processes incoming messages. */
static thread_t ethernet_incoming_thread = {
        .name       = "Incoming Ethernet Thread",  /* Name */
        .size       = 512,                         /* Stack Size (in bytes) */
        .priority   = PRIO_vEthernetIncoming,      /* Priority */
        .threshold  = 0,                           /* Preemption Threshold */
        .time_slice = TX_NO_TIME_SLICE,            /* Time Slice */
        .auto_start = TX_AUTO_START,               /* Auto Start */
        .sleep      =  0,                          /* Sleep (in ticks) */
        .function   = vEthernetIncoming            /* Thread Function */
    };
void vEthernetIncoming(ULONG thread_input) {

    while(1) {

        ethernet_message_t message;

        /* Process incoming messages */
        while(queue_receive(&eth_incoming, &message, TX_WAIT_FOREVER) == U_SUCCESS) {
            ethernet_inbox(&message);
        }

        /* No sleep. Thread timing is controlled completely by the queue timeout. */
    }
}

/* Outgoing Ethernet Thread. Sends outgoing messages. */
static thread_t ethernet_outgoing_thread = {
        .name       = "Outgoing Ethernet Thread",  /* Name */
        .size       = 512,                         /* Stack Size (in bytes) */
        .priority   = PRIO_vEthernetOutgoing,      /* Priority */
        .threshold  = 0,                           /* Preemption Threshold */
        .time_slice = TX_NO_TIME_SLICE,            /* Time Slice */
        .auto_start = TX_AUTO_START,               /* Auto Start */
        .sleep      =  0,                          /* Sleep (in ticks) */
        .function   = vEthernetOutgoing            /* Thread Function */
    };
void vEthernetOutgoing(ULONG thread_input) {

    while(1) {

        ethernet_message_t message;
        uint8_t status;

        /* Send outgoing messages */
        while(queue_receive(&eth_outgoing, &message, TX_WAIT_FOREVER) == U_SUCCESS) {
            status = ethernet_send_message(&message);
            if(status != U_SUCCESS) {
                PRINTLN_WARNING("Failed to send Ethernet message after removing from outgoing queue (Message ID: %d).", message.message_id);
                // u_TODO - maybe add the message back into the queue if it fails to send? not sure if this is a good idea tho
                }
        }

        /* No sleep. Thread timing is controlled completely by the queue timeout. */
    }
}