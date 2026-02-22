#include "u_threads.h"
#include "u_tx_debug.h"
#include "u_sensors.h"
#include "timer.h"
#include "tx_api.h"
#include "u_can.h"
#include "u_queues.h"
#include "main.h"
#include "u_inbox.h"

#define PRIO_DEFAULT          0
#define PRIO_CAN_INCOMING     0
#define PRIO_CAN_OUTGOING     0
#define PRIO_SENSORS          1

/* Default Thread */
static thread_t _default_thread = {
        .name       = "Default Thread",  /* Name */
        .size       = 2048,              /* Stack Size (in bytes) */
        .priority   = PRIO_DEFAULT,      /* Priority */
        .threshold  = 0,                 /* Preemption Threshold */
        .time_slice = TX_NO_TIME_SLICE,  /* Time Slice */
        .auto_start = TX_AUTO_START,     /* Auto Start */
        .sleep      = 50,                /* Sleep (in ticks) */
        .function   = default_thread     /* Thread Function */
    };
void default_thread(ULONG thread_input) {
    
    while(1) {
        /* Kick watch dog */
        HAL_IWDG_Refresh(&hiwdg);

        /* Sleep Thread for specified number of ticks. */
        tx_thread_sleep(_default_thread.sleep);
    }
}

/* CAN Incoming Thread. Processes incoming messages. */
static thread_t _can_incoming_thread = {
        .name       = "CAN Incoming Thread",     /* Name */
        .size       = 2048,                      /* Stack Size (in bytes) */
        .priority   = PRIO_CAN_INCOMING,         /* Priority */
        .threshold  = 0,                         /* Preemption Threshold */
        .time_slice = TX_NO_TIME_SLICE,          /* Time Slice */
        .auto_start = TX_AUTO_START,             /* Auto Start */
        .sleep      = 10,                        /* Sleep (in ticks) */
        .function   = can_incoming_thread        /* Thread Function */
    };
void can_incoming_thread(ULONG thread_input) {
    
    while(1) {
        can_msg_t message;

        /* Process incoming messages */
        while(queue_receive(&can_incoming, &message, TX_WAIT_FOREVER) == U_SUCCESS) {
            inbox_can(&message);
        }

        tx_thread_sleep(_can_incoming_thread.sleep);
    }
}

/* CAN Outgoing Thread. Sends outgoing CAN messages. */
static thread_t _can_outgoing_thread = {
    .name       = "CAN Outgoing Thread",     /* Name */
    .size       = 2048,                      /* Stack Size (in bytes) */
    .priority   = PRIO_CAN_OUTGOING,         /* Priority */
    .threshold  = 0,                         /* Preemption Threshold */
    .time_slice = TX_NO_TIME_SLICE,          /* Time Slice */
    .auto_start = TX_AUTO_START,             /* Auto Start */
    .sleep      = 10,                        /* Sleep (in ticks) */
    .function   = can_outgoing_thread        /* Thread Function */
};
void can_outgoing_thread(ULONG thread_input) {

    while(1) {

        can_msg_t message;
        HAL_StatusTypeDef status;

        /* Process outgoing messages */
        while(queue_receive(&can_outgoing, &message, TX_WAIT_FOREVER) == U_SUCCESS) {
            status = can_send_msg(&can2, &message);
            if(status != HAL_OK) {
                PRINTLN_WARNING("WARNING: Failed to send message (on can2) after removing from outgoing queue (Message ID: %ld).", message.id);
            }
        }

        tx_thread_sleep(_can_outgoing_thread.sleep);
    }
}

/* Sensors Thread */
static nertimer_t data_send_timer;
static thread_t _sensors_thread = {
    .name       = "Sensors Thread",  /* Name */
    .size       = 2048,              /* Stack Size (in bytes) */
    .priority   = PRIO_SENSORS,      /* Priority */
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
    CATCH_ERROR(create_thread(byte_pool, &_can_incoming_thread), U_SUCCESS); // Create CAN Incoming thread.
    CATCH_ERROR(create_thread(byte_pool, &_can_outgoing_thread), U_SUCCESS); // Create CAN Outgoing thread.


    PRINTLN_INFO("Ran threads_init().");
    return U_SUCCESS;
}