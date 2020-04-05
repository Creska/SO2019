#ifndef BIKAYA_PHASE0_SCHEDULER_H
#define BIKAYA_PHASE0_SCHEDULER_H

#include "core/system.h"
#include "pcb.h"

#define SCHEDULER_TIME_SLICE 3000   // in microseconds
#define SCHED_PRIORITY_INC 1


// Initialization routine for the scheduler
void init_scheduler();

// The method called right after an interval timer interrupt
void on_scheduler_callback();

// Returns the number of clock ticks per time slice as calculated during initialization.
// If called before scheduler initialization returns 0.
unsigned int get_ticks_per_slice();

// Adds a process to the ready queue
// If the added process has an higher priority than the running process the furst is executed right away and the second goes back to the ready queue
pcb_t* add_process(void* method, unsigned int priority, unsigned int vm_on, unsigned int km_on, unsigned int int_timer_on, unsigned int other_int_on);

// Launches the process with the higher priority in the ready queue
void launch();

// Returns a pointer to the running PCB
pcb_t* get_running_proc();

// Terminates the running process and removes it and its offspring from the ready queue
void syscall3();

#endif //BIKAYA_PHASE0_SCHEDULER_H
