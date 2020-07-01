#ifndef BIKAYA_SCHEDULER_H
#define BIKAYA_SCHEDULER_H

#include "core/processes/pcb.h"
#include "core/processes/process_initialization_data.h"
#include "devices/devices.h"

#define PRIORITY_INC_PER_TIME_SLICE     1     // The amount the priority of each process in the ready queue is increased every time slice


// External device waiting list.
// Processes that want to use external devices enqueue on this structures with the following logic:
//      - if the device is not busy the command is sent and the process' pointer is saved in w_for_res while waiting for the response
//      - if the device is busy the process is enqueued on w_for_cmd_sem
//      - when the device is done with the first command, sends the cmd for the first proc in the sem queue, sets it as w_for_res (removing it from the sem queue) and so on
typedef struct dev_waiting_list {
    int w_for_cmd_sem;
    pcb_t* w_for_res;
} dev_w_list;


// Returns the external device waiting list for the device identified by the given dev_type and dev_number
dev_w_list* get_dev_w_list(enum ext_dev_type dev_type, unsigned int instance);


// INITIALIZATION -----------------------------------------------------------------------------------------------------

// Initialization routine for the scheduler
void init_scheduler(proc_init_data starting_procs[], unsigned int procs_number, unsigned int time_slice);

// Launches the process with the higher priority in the ready queue, panics if the ready queue is empty.
void launch();

// PROCESS MANAGEMENT -------------------------------------------------------------------------------------------------

// Resets the interval timer value to the number of ticks corresponding to the correct time-slice.
//
// Remarks: this also acknowledges an interval timer interrupt if pending
void reset_int_timer();

// Returns a pointer to the running PCB
pcb_t* get_running_proc();

// Returns a pointer to the idle process
pcb_t* get_idle_proc();

// Returns the amount of clock ticks corresponding to a full time slice
unsigned int get_clock_ticks_per_time_slice();

// Sets as running the first proc in the ready queue (idle if empty) and returns a pointer to the process that was running WITHOUT returning it to the ready queue
pcb_t* pop_running();

// Schedules a new process for execution. If it has an higher priority than the running process
// it is executed immediately, otherwise it goes in the ready queue.
void schedule_proc(pcb_t* p);

//Runs a new proc and check if it is the idle_proc to avoid returning it to the ready queue
void swap_running_proc(pcb_t* new_proc);

// Debug-prints the ready queue
void debug_ready_queue();

// Exposed idle method used by the idle process. It's just an infinite loop.
void idle();


// PCB TIMERS ---------------------------------------------------------------------------------------------------------

// Adds the time passed since the last flush to the user time of the given process
void flush_user_time(pcb_t* proc);

// Adds the time passed since the last flush to the kernel time of the given process
void flush_kernel_time(pcb_t* proc);

// Resets the "last flush" cached TOD to now
void flush_reset_time(pcb_t* proc);



// EXCEPTION HANDLING -------------------------------------------------------------------------------------------------

// The method called while handling an interval timer interrupt
void time_slice_callback();

// Terminates the given process (the running one if p==NULL) and all its offspring
// Returns 0 if the operation is successful, -1 otherwise
int terminate_proc(pcb_t* p);

// Creates a new process as child of the running one.
//
// s:           contains the initial state of the process
// priority:    is the priority of the new process
// cpid:        is the location of the (pcb_t*) variable where the new process id should be saved
int create_process(state_t* s, int priority, pcb_t** cpid);

// Passeren
void p(int* semaddr);

// Verhogen
void v(int* semaddr);

//Return the process' index. If it is the idle_proc return the MAX_PROC's macro to recognize it
unsigned int get_proc_scheduler_index(pcb_t* p);

#endif //BIKAYA_SCHEDULER_H
