#ifndef BIKAYA_SCHEDULER_H
#define BIKAYA_SCHEDULER_H

#include "core/processes/pcb.h"
#include "core/processes/process_initialization_data.h"
#include "devices/devices.h"



typedef struct dev_waiting_list {
    int sem;
    pcb_t* w_for_res;
} dev_w_list;

dev_w_list* get_dev_w_list(enum ext_dev_type dev_type, unsigned int instance);




#define PRIORITY_INC_PER_TIME_SLICE     1               // The amount the priority of each process in the ready queue is increased every time slice

// Initialization-related methods -------------------------------------------------------------------------------------

// Initialization routine for the scheduler
void init_scheduler(proc_init_data starting_procs[], unsigned int procs_number, unsigned int time_slice);

// Launches the process with the higher priority in the ready queue, panics if the ready queue is empty.
void launch();

// --------------------------------------------------------------------------------------------------------------------

// Resets the interval timer value to the number of ticks corresponding to the correct time-slice.
//
// Remarks: this also acknowledges an interval timer interrupt if pending
void reset_int_timer();

// Returns a pointer to the running PCB
pcb_t* get_running_proc();

// Returns a pointer to the idle process
pcb_t* get_idle_proc();

// Sets as running the first proc in the ready queue and returns the process that was running previously
pcb_t* swap_running();

// Schedules a new process for execution. If it has an higher priority than the running process
// it is executed immediately, otherwise it goes in the ready queue.
void schedule_proc(pcb_t* p);

void debug_ready_queue();

void idle();


// Exception handling methods -----------------------------------------------------------------------------------------

// The method called while handling an interval timer interrupt
void time_slice_callback();

// Terminates the given process (the running one if p==NULL) and all its offspring
// Returns 0 if the operation is successfull, -1 otherwise
int terminate_proc(pcb_t* p);

// Creates a new process as child of the running one.
//
// s:           contains the initial state of the process
// priority:    is the priority of the new process
// cpid:        is the location of the (pcb_t*) variable where the new process id should be saved
int create_process(state_t* s, int priority, pcb_t** cpid);

void p(int* semaddr);

void v(int* semaddr);

#endif //BIKAYA_SCHEDULER_H
