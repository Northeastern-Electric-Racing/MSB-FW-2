#ifndef __U_STEERING_ANGLE_H
#define __U_STEERING_ANGLE_H

#include "u_adcs.h"

typedef struct {
    float angle[NUM_STEERING_ANGLES]; 
} steering_angle_data_t;

steering_angle_data_t steering_angle_get_data();

#endif /* u_steering_angle.h */