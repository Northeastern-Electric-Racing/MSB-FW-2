#ifndef __U_SHOCK_POT_H
#define __U_SHOCK_POT_H

#include "u_adcs.h"

typedef struct {
    float position[NUM_SHOCK_POTS]; 
} shock_pot_data_t;

shock_pot_data_t shock_pot_get_data();

#endif /* u_shock_pot.h */