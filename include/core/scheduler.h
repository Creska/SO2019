#ifndef BIKAYA_PHASE0_SCHEDULER_H
#define BIKAYA_PHASE0_SCHEDULER_H


#define SCHEDULER_TIME_SLICE 300000

#include "core/system.h"


// Initialization routine for the scheduler
void init_scheduler();

// The method called right after an interval timer interrupt
// Takes the state_t* of the process that was interrupted and returns the state_t* of the process that needs to be resumed
state_t* time_slice_expired_callback(state_t* interrupted_proc);

// Returns the number of clock ticks per time slice as calculated during initialization.
// If called before scheduler initialization returns 0.
unsigned int get_ticks_per_slice();


#endif //BIKAYA_PHASE0_SCHEDULER_H
