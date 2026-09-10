#include <stdint.h>
#include "u_adcs.h"
#include "u_steering_angle.h"
#include "u_utils.h"
#include "u_can.h"
#include "u_queues.h"

#define STEERING_ANGLE1_ZERO_OFFSET   0.0f
#define STEERING_ANGLE1_SCALE_FACTOR  1.0f

#define V_MIN      1.0626f   // fully right -> +87 deg
#define V_STRAIGHT 1.80f     // center      ->   0 deg
#define V_MAX      2.434f    // fully left  -> -75 deg

#define ANGLE_MIN   87.0f    // magnitude at V_MIN
#define ANGLE_MAX   75.0f    // magnitude at V_MAX

float steering_voltage_to_angle(float v)
{
    // Clamp to sensor's physical range to avoid garbage on fault/disconnect
    if (v < V_MIN) v = V_MIN;
    if (v > V_MAX) v = V_MAX;

    float angle;

    if (v <= V_STRAIGHT) {
        // Right side: V_MIN (+87) -> V_STRAIGHT (0)
        angle = ANGLE_MIN * (V_STRAIGHT - v) / (V_STRAIGHT - V_MIN);
    } else {
        // Left side: V_STRAIGHT (0) -> V_MAX (-75)
        angle = -ANGLE_MAX * (v - V_STRAIGHT) / (V_MAX - V_STRAIGHT);
    }

    return angle; // degrees, +87 = full right, -75 = full left
}

steering_angle_data_t steering_angle_get_data() {
    raw_steering_angle_adc_t rawData = adc_get_steering_angle_data();

    float steeringAngle1Volts = adc_to_voltage(rawData.data[STEERING_ANGLE1]);

    steering_angle_data_t sp;
    sp.angle[STEERING_ANGLE1] = steering_voltage_to_angle(adc_calibrate(steeringAngle1Volts, STEERING_ANGLE1_ZERO_OFFSET, STEERING_ANGLE1_SCALE_FACTOR));

    return sp;
}

void send_steering_angle_data(steering_angle_data_t data) {
    struct __attribute__((__packed__)) {
		int32_t steering_angle_1;
	} steering_angle_data;

    steering_angle_data.steering_angle_1 = data.angle[STEERING_ANGLE1];

    can_msg_t can_message = {.id = STEERING_ANGLE_CAN_ID, .len = 4, .data = {0}};

    memcpy(can_message.data, &steering_angle_data, can_message.len);

    queue_send(&can_outgoing, &can_message, TX_NO_WAIT);
}