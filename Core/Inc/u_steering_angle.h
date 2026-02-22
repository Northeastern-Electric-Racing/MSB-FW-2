#ifndef __U_STEERING_ANGLE_H
#define __U_STEERING_ANGLE_H

#include "u_adcs.h"

/** ADC data from Steering Angle ADC */
typedef struct {
    float angle[NUM_STEERING_ANGLES]; /** Calibrated and converted ADC data */
} steering_angle_data_t;

/**
 * @brief Reads the adc steering angle value and returns the processed data
 * @return Calibrated data from steering angle
 */
steering_angle_data_t steering_angle_get_data();

#endif /* u_steering_angle.h */