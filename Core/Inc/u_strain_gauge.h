#ifndef __U_STRAIN_GAUGE_H
#define __U_STRAIN_GAUGE_H

#include "u_adcs.h"

typedef struct {
    float strain[NUM_STRAIN_GAUGES]; 
} strain_gauge_data_t;

strain_gauge_data_t strain_gauge_get_data();

#endif /* u_strain_guages.h */