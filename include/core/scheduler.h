#ifndef BIKAYA_PHASE0_SCHEDULER_H
#define BIKAYA_PHASE0_SCHEDULER_H

#include "core/system.h"
#include "pcb.h"

#define SCHEDULER_TIME_SLICE 3000


// Initialization routine for the scheduler
void init_scheduler();

// The method called right after an interval timer interrupt
void on_scheduler_callback();

// Returns the number of clock ticks per time slice as calculated during initialization.
// If called before scheduler initialization returns 0.
unsigned int get_ticks_per_slice();

pcb_t* add_process(void* method, unsigned int priority, unsigned int vm_on, unsigned int km_on, unsigned int int_on);

// temp function that runs the head process of the ready queue
void launch();

pcb_t* get_running_proc();

#endif //BIKAYA_PHASE0_SCHEDULER_H
