#ifndef BIKAYA_BIKAYA_H
#define BIKAYA_BIKAYA_H

#include "core/processes/process_initialization_data.h"

struct bikaya_initialiation_data {
    unsigned int time_slice;
    proc_init_data* starting_procs_data;
    unsigned int starting_procs_n;
};

typedef struct bikaya_initialiation_data bikaya_init_data;

void bikaya_launch(bikaya_init_data* init_data);

void bikaya_add_proc(proc_init_data* new_proc);

#endif //BIKAYA_BIKAYA_H
