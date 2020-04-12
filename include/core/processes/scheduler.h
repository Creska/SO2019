#ifndef BIKAYA_PHASE0_SCHEDULER_H
#define BIKAYA_PHASE0_SCHEDULER_H

#include "core/system/system.h"
#include "core/processes/pcb.h"

#ifdef DEBUG
#define SCHEDULER_TIME_SLICE            30000            // in microseconds (increased in DEBUG mode to minimize risk of interval timer interruptions while logging during process execution)
#else
#define SCHEDULER_TIME_SLICE            3000            // in microseconds
#endif


#define PRIORITY_INC_PER_TIME_SLICE     1               // The amount the priority of each process in the ready queue is increased every time slice


// Initialization routine for the scheduler
void init_scheduler();

// The method called right after an interval timer interrupt
void time_slice_callback();

// Returns the number of clock ticks per time slice as calculated during initialization.
// If called before scheduler initialization returns 0.
unsigned int get_ticks_per_slice();

// Adds a process to the ready queue
// If the added process has an higher priority than the running process the first is executed right away and the second goes back to the ready queue
pcb_t* add_process(void* method, int priority, unsigned int vm_on, unsigned int km_on, unsigned int int_timer_on, unsigned int other_int_on);

// Launches the process with the higher priority in the ready queue, panics if the ready queue is empty.
void launch();

// Returns a pointer to the running PCB
pcb_t* get_running_proc();

// Terminates the running process and removes it and its offspring from the ready queue
// returns the process state that needs to be resumed after the operation
state_t* syscall3();

#endif //BIKAYA_PHASE0_SCHEDULER_H
