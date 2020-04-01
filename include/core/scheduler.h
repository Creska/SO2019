#ifndef BIKAYA_PHASE0_SCHEDULER_H
#define BIKAYA_PHASE0_SCHEDULER_H

#include "core/system.h"
#include "pcb.h"

#define SCHEDULER_TIME_SLICE 30000


// Initialization routine for the scheduler
void init_scheduler();

// The method called right after an interval timer interrupt
// Takes the state_t* of the process that was interrupted and returns the state_t* of the process that needs to be resumed
void time_slice_expired_callback();

// Returns the number of clock ticks per time slice as calculated during initialization.
// If called before scheduler initialization returns 0.
unsigned int get_ticks_per_slice();

void add_process(void* method, unsigned int priority, unsigned int vm_on, unsigned int km_on, unsigned int int_on);

// temp function that runs the head process of the ready queue
void run();

#endif //BIKAYA_PHASE0_SCHEDULER_H
