#ifndef BIKAYA_PHASE0_SCHEDULER_H
#define BIKAYA_PHASE0_SCHEDULER_H

#include "core/system/system.h"
#include "core/processes/pcb.h"
#include "core/processes/process_initialization_data.h"

#ifdef DEBUG
#define SCHEDULER_TIME_SLICE            30000            // in microseconds (increased in DEBUG mode to minimize risk of interval timer interruptions while logging during process execution)
#else
#define SCHEDULER_TIME_SLICE            3000            // in microseconds
#endif


#define PRIORITY_INC_PER_TIME_SLICE     1               // The amount the priority of each process in the ready queue is increased every time slice


// Initialization routine for the scheduler
void init_scheduler(proc_init_data starting_procs[], unsigned int procs_number);

// The method called right after an interval timer interrupt
void time_slice_callback();

// Adds a process to the ready queue
// If the added process has an higher priority than the running process the first is executed right away and the second goes back to the ready queue
pcb_t* add_process(proc_init_data* data);

// Resets the interval timer value to the number of ticks corresponding to the correct time-slice.
//
// Remarks: this also acknowledges an interval timer interrupt if pending
void reset_int_timer();

// Launches the process with the higher priority in the ready queue, panics if the ready queue is empty.
void launch();

// Returns a pointer to the running PCB
pcb_t* get_running_proc();

// Terminates the running process and removes its offspring from the ready queue
// returns the process state that needs to be resumed after the operation
void terminate_running_proc();


#endif //BIKAYA_PHASE0_SCHEDULER_H
