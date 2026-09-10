#include "u_threads.h"
#include "timer.h"
#include "tx_api.h"
#include "u_can.h"
#include "u_queues.h"
#include "main.h"
#include "u_inbox.h"
#include "u_thermocouple.h"
#include "u_strain_gauge.h"
#include "u_shock_pot.h"
#include "u_steering_angle.h"
#include "u_load_cell.h"
#include "u_misc_adc.h"
#include "u_utils.h"
#include "u_sensors.h"
#include "u_wheel_speed.h"
#include "can_messages_tx.h"
#include "u_tx_general.h"

#define PRIO_DEFAULT          0
#define PRIO_CAN_INCOMING     0
#define PRIO_CAN_OUTGOING     0
#define PRIO_SENSORS          1
#define PRIO_ADCS             1
#define PRIO_WHEEL_SPEED      1

/* Default Thread */
static thread_t _default_thread = {
        .name       = "Default Thread",  /* Name */
        .size       = 2048,              /* Stack Size (in bytes) */
        .priority   = PRIO_DEFAULT,      /* Priority */
        .threshold  = 0,                 /* Preemption Threshold */
        .time_slice = TX_NO_TIME_SLICE,  /* Time Slice */
        .auto_start = TX_AUTO_START,     /* Auto Start */
        .sleep      = 200,                /* Sleep (in ticks) */
        .function   = default_thread     /* Thread Function */
    };
void default_thread(ULONG thread_input) {
    
    bool alt = true;

    while(1) {
        /* Kick watch dog */
        HAL_IWDG_Refresh(&hiwdg);

        if (alt) {
			printf(".\n");
		} else {
			printf("..\n");
		}

        alt = !alt;

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

    while (1) {
        // read_imu_and_magnometer();
        // wheel_pulse_check();

        if (is_timer_expired(&data_send_timer)) {
            read_hdc2021();
            // read_ssc();
            // send_hdc2021_data();
            // send_ssc_data();
            // send_imu_and_magnometer_data();

            if (device_loc == DEVICE_BACK) {
                // read_vl53l7cx();
                // send_vl53l7cx_data();
            }

            start_timer(&data_send_timer, DATA_SEND_INTERVAL);
        }

        tx_thread_sleep(_sensors_thread.sleep / 2);

        // prepare_data_hdc2021();

        tx_thread_sleep(_sensors_thread.sleep / 2);
    }
}

/* Wheel Speed Thread */
static thread_t _wheel_speed_thread = {
    .name       = "Wheel Speed Thread",
    .size       = 2048,
    .priority   = PRIO_WHEEL_SPEED,
    .threshold  = 0,
    .time_slice = TX_NO_TIME_SLICE,
    .auto_start = TX_AUTO_START,
    .sleep      = MS_TO_TICKS(25U),
    .function   = wheel_speed_thread
};
void wheel_speed_thread(ULONG thread_input) {

    wheel_speed_init(&htim1, &htim15);
    tx_thread_sleep(_wheel_speed_thread.sleep);

    while (1) {
        wheel_pulse_check();
        wheel_speed_data_t data = wheel_speed_get_data();
        switch (device_loc) {
            case DEVICE_FRONT:
                send_front_wheel_rpm(data.left_rpm, data.right_rpm);
                break;

            case DEVICE_BACK:
                /* Back wheel RPM CAN message is not defined yet. */
                break;
        }

        printf("Wheel speed: left=%u RPM, right=%u RPM\r\n",
               (unsigned int)data.left_rpm,
               (unsigned int)data.right_rpm);

        tx_thread_sleep(_wheel_speed_thread.sleep);
    }
}

/* ADCs Thread */
static thread_t _adcs_thread = {
    .name       = "ADCs Thread",     /* Name */
    .size       = 2048,              /* Stack Size (in bytes) */
    .priority   = PRIO_ADCS,         /* Priority */
    .threshold  = 0,                 /* Preemption Threshold */
    .time_slice = TX_NO_TIME_SLICE,  /* Time Slice */
    .auto_start = TX_AUTO_START,     /* Auto Start */
    .sleep      = MS_TO_TICKS(10U),  /* Shock-pot update period */
    .function   = adcs_thread     /* Thread Function */
};
void adcs_thread(ULONG thread_input) {

    adc_init();

    while(1) {
        // if (device_loc == DEVICE_BACK) {
        //     thermocouple_data_t thermo_data = thermocouple_get_data();
        //     send_thermocouple_data(thermo_data);
        // }

        // strain_gauge_data_t strain_gauge_data = strain_gauge_get_data();
        // send_strain_gauge_data(strain_gauge_data);
        
        // load_cell_data_t load_cell2_data = load_cell2_get_data();

        // misc_adc_data_t misc_adc2_data = misc_adc2_get_data();
        // send_misc_adc_data(misc_adc2_data, MISC_ADC2_CAN_ID);

        // tx_thread_sleep(_sensors_thread.sleep / 4);

        adc_switchMuxStates(LOW);

        // tx_thread_sleep(_sensors_thread.sleep / 4);

        shock_pot_data_t shock_pot_data = shock_pot_get_data();
        switch (device_loc) {
            case DEVICE_FRONT:
                send_front_left_shockpot(shock_pot_data.inch_travel[SHOCK_POT1],
                                         shock_pot_data.raw_adc[SHOCK_POT1]);
                send_front_right_shockpot(shock_pot_data.inch_travel[SHOCK_POT2],
                                          shock_pot_data.raw_adc[SHOCK_POT2]);
                break;

            case DEVICE_BACK:
                /* Back shock-pot CAN messages are not defined yet. */
                break;
        }


        steering_angle_data_t steering_angle_data = steering_angle_get_data();
        PRINTLN_INFO("STEERING ANGLE RAW VOLTAGE: %f", steering_angle_data.angle[STEERING_ANGLE1]);



        // if (device_loc == DEVICE_FRONT) {
        //     steering_angle_data_t steering_angle_data = steering_angle_get_data();
        //     send_steering_angle_data(steering_angle_data);
        // }

        // load_cell_data_t load_cell1_data = load_cell1_get_data();
        // send_load_cell_data(load_cell1_data, load_cell2_data);

        // misc_adc_data_t misc_adc1_data = misc_adc1_get_data();
        // send_misc_adc_data(misc_adc1_data, MISC_ADC1_CAN_ID);
        // misc_adc_data_t misc_adc3_data = misc_adc3_get_data();
        // send_misc_adc_data(misc_adc3_data, MISC_ADC3_CAN_ID);

        // tx_thread_sleep(_sensors_thread.sleep / 4);

        // adc_switchMuxStates(HIGH);

        tx_thread_sleep(_adcs_thread.sleep);
    }
}

/* Initializes all ThreadX threads. 
*  Calls to create_thread() should go in here
*/
uint8_t threads_init(TX_BYTE_POOL *byte_pool) {

    /* Create Threads */
    CATCH_ERROR(create_thread(byte_pool, &_default_thread), U_SUCCESS);      // Create Default thread.
    // CATCH_ERROR(create_thread(byte_pool, &_sensors_thread), U_SUCCESS);      // Create Sensors thread.
    CATCH_ERROR(create_thread(byte_pool, &_can_incoming_thread), U_SUCCESS); // Create CAN Incoming thread.
    CATCH_ERROR(create_thread(byte_pool, &_can_outgoing_thread), U_SUCCESS); // Create CAN Outgoing thread.
    CATCH_ERROR(create_thread(byte_pool, &_adcs_thread), U_SUCCESS);         // Create ADCs thread.
    CATCH_ERROR(create_thread(byte_pool, &_wheel_speed_thread), U_SUCCESS);  // Create Wheel Speed thread.

    PRINTLN_INFO("Ran threads_init().");
    return U_SUCCESS;
}
