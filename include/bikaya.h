#ifndef BIKAYA_BIKAYA_H
#define BIKAYA_BIKAYA_H

#include "core/processes/process_initialization_data.h"


void bikaya_launch(proc_init_data starting_procs[], unsigned int procs_number);

void bikaya_add_proc(proc_init_data* new_proc);

#endif //BIKAYA_BIKAYA_H
