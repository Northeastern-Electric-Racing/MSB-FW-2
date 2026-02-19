#ifndef __U_LOAD_CELL_H
#define __U_LOAD_CELL_H

#include "u_adcs.h"

typedef struct {
    float force[NUM_LOAD_CELLS]; 
} load_cell_data_t;

load_cell_data_t load_cell1_get_data();
load_cell_data_t load_cell2_get_data();

#endif /* u_load_cell.h */