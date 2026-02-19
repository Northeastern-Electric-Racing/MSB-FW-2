#ifndef __U_THERMOCOUPLE_H
#define __U_THERMOCOUPLE_H

#include "u_adcs.h"

typedef struct {
    float temp[NUM_THERMOCOUPLES];
} thermocouple_data_t;

thermocouple_data_t thermocouple_get_data();

#endif /* u_threads.h */