#include <stdint.h>
#include "u_adcs.h"
#include "u_steering_angle.h"
#include "u_utils.h"

#define STEERING_ANGLE1_ZERO_OFFSET   0.0f
#define STEERING_ANGLE1_SCALE_FACTOR  1.0f

steering_angle_data_t steering_angle_get_data() {
    raw_steering_angle_adc_t rawData = adc_get_steering_angle_data();

    float steeringAngle1Volts = adc_to_voltage(rawData.data[STEERING_ANGLE1]);

    steering_angle_data_t sp;
    sp.angle[STEERING_ANGLE1] = adc_calibrate(steeringAngle1Volts, STEERING_ANGLE1_ZERO_OFFSET, STEERING_ANGLE1_SCALE_FACTOR);

    return sp;
}