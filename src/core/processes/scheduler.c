#include "utils/utils.h"
#include "core/processes/scheduler.h"
#include "core/system/system.h"
#include "utils/debug.h"
#include "core/processes/asl.h"

// Cached value (calculated at initialization) for how the length of a time_slice (in ticks per microsecond)
unsigned int clock_ticks_per_time_slice;

// Procs ready queue, holding processes ready to be run
struct list_head ready_queue;

// Pointer to the running process
pcb_t* running_proc = NULL;

// Dummy process run by the system when waiting for all other processes to resume
pcb_t idle_proc;

// Semaphores for device waiting. One for each distinct device
dev_w_list dev_w_lists[N_EXT_IL + 1][N_DEV_PER_IL];



// Private utils ======================================================================================================

// Retrieves a pcb from the freePCB list, panics if the freePCB list is empty, meaning that all the pcbs are already in use
pcb_t* get_free_pcb_else_panic();

// Returns the runing process to the ready queue (if the running process is the idle process is ignored)
void stop_running_proc();

// Sets the given process as running, if NULL is given the idle process is run
void set_running_proc(pcb_t* new_proc);

// Populates a pcb with the given data
//
// WARNING: the pcb is assumed to have all the fields initialized to 0/NULL (for example retrieved through get_free_pcb),
// if not might cause undefined behaviour since for efficiency this methods overrides some of the values only when target values are non-zero/non-NULL
void populate_pcb(pcb_t* p, proc_init_data* data);

int recursive_remove_proc(pcb_t* p);



// Initialization =====================================================================================================

void init_scheduler(proc_init_data starting_procs[], unsigned int procs_number, unsigned int time_slice) {

#ifdef DEBUG            // If in DEBUG mode increase timeslice in order to avoid logs consuming the whole time slice
    time_slice = time_slice*10;
#endif

    clock_ticks_per_time_slice = clock_ticks_per_period(time_slice);                      // This value will be the same until reboot or reset, we can just cache it
    initPcbs();
    initASL();
    mkEmptyProcQ(&ready_queue);

    for (int i = 0; i < procs_number; ++i) {        // For each starting proc populate a PCB and insert it in the ready queue
        pcb_t* p = get_free_pcb_else_panic();
        populate_pcb(p, &starting_procs[i]);
        insertProcQ(&ready_queue, p);
    }

    // Idle process setup
    reset_state(&idle_proc.p_s);
    proc_init_data idle_proc_data = {.km_on=1, .method=idle, .timer_int_on=1, .other_ints_on=1, .vm_on=0, .priority = 0};
    populate_pcb(&idle_proc, &idle_proc_data);

    for (int l = 0; l < N_EXT_IL+1; ++l) {                   // Initialize every external device waiting lists' semaphores to 1
        for (int d = 0; d < N_DEV_PER_IL; ++d) {
            dev_w_lists[l][d].w_for_cmd_sem = 1;
            dev_w_lists[l][d].w_for_res = NULL;
        }
    }
}

void launch() {
    if (!emptyProcQ(&ready_queue)) {
        set_running_proc(removeProcQ(&ready_queue));

        running_proc->tod_at_start = TOD;
        running_proc->tod_cache = TOD;


        DEBUG_LOG_INT("LAUNCHING PROCESS WITH ID: ", get_process_index(running_proc));
        DEBUG_SPACING;

        reset_int_timer();
        LDST(&running_proc->p_s);
    } else {
        adderrbuf("Impossible to launch the system, no processes in the ready queue");
    }
}

// Interval timer ----------------------------------------------------------------------

void reset_int_timer() {
    DEBUG_LOG_INT("Resetting interval timer to ", clock_ticks_per_time_slice);
    set_interval_timer(clock_ticks_per_time_slice);
}

// Process management -------------------------------------------------------------------------------------------------

void idle() {
    while (1) {}
}

pcb_t *get_running_proc() {
    return running_proc;
}

pcb_t* get_idle_proc() {
    return &idle_proc;
}

unsigned int get_clock_ticks_per_time_slice() { return clock_ticks_per_time_slice; }

void schedule_proc(pcb_t* p) {
    DEBUG_LOG_INT("Scheduling process: ", get_process_index(p));
    if (p->priority > running_proc->priority || running_proc==&idle_proc) {
        DEBUG_LOG("The newly created process has higher priority than the running one, swapping them instantly");
        stop_running_proc();
        set_running_proc(p);
    } else {
        DEBUG_LOG ("The newly created process has lower priority than the running one, just inserting it in the ready queue");
        insertProcQ(&ready_queue, p);
    }
}

int create_process(state_t *s, int priority, pcb_t **cpid) {
    pcb_t* p = allocPcb();
    if (p!=NULL) {
        DEBUG_LOG_INT("Creating new process: ", get_process_index(p));
        memcpy(&p->p_s, s, sizeof(state_t));
        p->priority = priority;
        p->original_priority = priority;
        insertChild(running_proc, p);               // Set the new process as child of the running one
        if (cpid!=NULL) {(*cpid) = p;}
        DEBUG_LOG("Ending creation");
        schedule_proc(p);
        return 0;
    } else {
        DEBUG_LOG("Maximum number of processes was reached, failed to create a new one");
        return -1;
    }
}

int terminate_proc(pcb_t *p) {
    DEBUG_LOG_INT("Termination entry point for proc ", get_process_index(p));
    if (p==NULL) { p = running_proc; }

    if (p->p_parent!=NULL) {
        outChild(p);
    }

    insertProcQ(&ready_queue, running_proc);                                        // Insert temporarily the running proc in the ready queue to facilitate recursion and removal checks
    int ret = recursive_remove_proc(p);

    pcb_t* retrieved_running_proc = outProcQ(&ready_queue, running_proc);           // Take back the running process from the ready queue
    if (retrieved_running_proc==NULL) {                                             // If the running process wasn't in the ready queue at this point it means that it was removed
        pcb_t* new_proc = removeProcQ(&ready_queue);
        if (new_proc!=NULL) {
            set_running_proc(new_proc);                                             // so we run a new process if the ready queue isn't empty
        } else {
            set_running_proc(&idle_proc);
        }
    }
    return ret;
}

void debug_ready_queue() {
    struct pcb_t* target_proc;
    DEBUG_LOG("Ready queue:");
    list_for_each_entry(target_proc, &ready_queue, p_next) {                    // Increments the priority of each process in the ready_queue (anti-starvation measure)
        DEBUG_LOG_INT("\tproc ", get_process_index(target_proc));
    }
}

pcb_t *pop_running() {
    pcb_t* prev_running = running_proc;
    set_running_proc(removeProcQ(&ready_queue));

    DEBUG_LOG_UINT("Swapped proc: ", get_process_index(prev_running));
    DEBUG_LOG_UINT("...with proc: ", get_process_index(running_proc));
    return prev_running;
}

dev_w_list* get_dev_w_list(enum ext_dev_type dev_type, unsigned int instance) {
    return &dev_w_lists[dev_type][instance];
}


// Exception handling -------------------------------------------------------------------------------------------------

void p(int* semaddr) {
    DEBUG_LOG_INT("P enter, semaphore has value ", *semaddr);
    DEBUG_LOG_UINT("Semaphore: ", (unsigned)semaddr);

    (*semaddr)--;

    if ((*semaddr)<0) {                                         // If there are no available resources
        flush_kernel_time(running_proc);                        // Count the time passed since the start of the syscall handler up to here as kernel time of this proc
        if (insertBlocked(semaddr, running_proc)) {             // blocks the process on the semaphore
            adderrbuf("No free SEMDs");
        }

        pcb_t* new_proc = removeProcQ(&ready_queue);       // Resume another process from the ready_queue
        if (new_proc!=NULL) {
            set_running_proc(new_proc);

        } else {
            set_running_proc(&idle_proc);                   // Since there's no ready process launch the idle process
        }
        (*semaddr)++;
    }
    DEBUG_LOG_INT("P exit, semaphore has value ", *semaddr);

}

void v(int* semaddr) {
    (*semaddr)++;
    pcb_t* dequeued_proc = removeBlocked(semaddr);
    if (dequeued_proc != NULL) {
        dequeued_proc->priority = dequeued_proc->original_priority;         // Restore the dequeued process' priority to the original
        semd_t* s = getSemd(semaddr);
        if (s!=NULL) {                                                          // If the semd still has processes in the queue...
            pcb_t* target;
            list_for_each_entry(target, &s->s_procQ, p_next) {            // ...increase the priority of all the processes
                target->priority += PRIORITY_INC_PER_TIME_SLICE;                // left on the semd queue in order to avoid starvation
            }
        }
        schedule_proc(dequeued_proc);

        (*semaddr)--;               // or p(semaddr)?
    }
    DEBUG_LOG("V exit");
}

void time_slice_callback() {
    DEBUG_LOG("Timeslice callback");
    struct pcb_t* target_proc;
    list_for_each_entry(target_proc, &ready_queue, p_next) {                    // Increments the priority of each process in the ready_queue (anti-starvation measure)
        DEBUG_LOG_INT("Increasing priority of process ", get_process_index(target_proc));
        target_proc->priority += PRIORITY_INC_PER_TIME_SLICE;
    }

    if (!list_empty(&ready_queue) && running_proc->priority <= headProcQ(&ready_queue)->priority) {             // Swap execution if the first ready process has a greater priority than the one executing (obviously if the ready queue is empty we don't need to swap)
        pcb_t* to_start = removeProcQ(&ready_queue);
        DEBUG_LOG_UINT("Swapping to process: ", get_process_index(to_start));

        stop_running_proc();// and set the first ready process as running (and remove it from the ready queue)
        set_running_proc(to_start);
    } else {
        if (list_empty(&ready_queue)) {
            DEBUG_LOG("The ready queue is empty, resuming current proc execution");
        } else {
            DEBUG_LOG("The current process still has the higher priority, resuming its execution");
        }
    }
    reset_int_timer();
}


// Private utils ======================================================================================================

pcb_t* get_free_pcb_else_panic() {
    pcb_t* p = allocPcb();
    if (p!=NULL) {
        return p;
    } else {
        adderrbuf("ERROR: The maximum number of processes has been reached!");
        return NULL;
    }
}

void populate_pcb(pcb_t* p, proc_init_data* data) {

    set_pc(&p->p_s, data->method);
    set_sp(&p->p_s, RAM_TOP - FRAME_SIZE*(get_proc_scheduler_index(p)+1));                     // Use the index of the process as index of the frame, this should avoid overlaps at any time
    set_interval_timer_interrupts(&p->p_s, data->timer_int_on);
    set_other_interrupts(&p->p_s, data->other_ints_on);
    p->priority = data->priority;
    p->original_priority = data->priority;

#ifdef TARGET_UMPS  // On umps we need to set the previous values since on process loading the vm, kernel and global interrupts stacks are popped
    set_virtual_mem(&p->p_s, 0);
    if (!data->km_on) { p->p_s.status = p->p_s.status | STATUS_KUp; }                                   // Kernel mode is on when the corresponding bit is 0
    if (data->vm_on) { p->p_s.status = p->p_s.status | STATUS_VMp; }
    if (data->timer_int_on || data->other_ints_on) { p->p_s.status = p->p_s.status | STATUS_IEp; }      // Manually sets the previous global interrupt switch
#elif TARGET_UARM  // On architectures without these mini-stacks (arm in our case) we can just set the values normally (using system.h methods)
    set_virtual_mem(&p->p_s, data->vm_on);
    set_kernel_mode(&p->p_s, data->km_on);
#endif
}

void stop_running_proc() {
    if (running_proc!=&idle_proc) {
        running_proc->priority = running_proc->original_priority;                                               // Reset the previously running process' priority to the original
        insertProcQ(&ready_queue, running_proc);// Insert the previously running process in the ready queue
    } else {
        DEBUG_LOG("The running process was the IDLE PROC, stopping it without putting it in the ready queue");
    }
}

void set_running_proc(pcb_t* new_proc) {
    if (new_proc) {
        running_proc = new_proc;
    } else {
        DEBUG_LOG_UINT("Setting IDLE PROC as running process: ", get_process_index(&idle_proc));
        running_proc = &idle_proc;
    }
}

int recursive_remove_proc(pcb_t* p) {

    pcb_t* to_be_freed = NULL;
    if (p->p_semkey!=NULL) {
        // The process is enqueued on a semaphore, remove it from the latter
        to_be_freed = outBlocked(p);
    } else if (p->dev_w_list!=NULL) {
        // The process is waiting for a response from a device
        to_be_freed = p;
        *p->dev_w_list = NULL;
    } else {
        // The process is in the ready queue, remove it
        to_be_freed = outProcQ(&ready_queue, p);
    }


    if (to_be_freed!=NULL) {
        freePcb(to_be_freed);
    } else {
        adderrbuf("Error during recursive process removal, the given process isn't on the ready queue or in a semaphore queue.");
        return -1;
    }

    pcb_t* target_child = removeChild(p);
    while (target_child!=NULL) {
        DEBUG_LOG_INT("Calling termination on child: ", get_process_index(target_child));
        if (recursive_remove_proc(target_child)) {
            return -1;              // There was some problem during termination, stop recursion and let the user handle it
        }
        target_child = removeChild(p);
    }
    return 0;
}

void flush_user_time(pcb_t* proc) {
    unsigned int cached_TOD = TOD;
    proc->user_timer += cached_TOD - proc->tod_cache;
    proc->tod_cache = cached_TOD;
}

void flush_kernel_time(pcb_t* proc) {
    unsigned int cached_TOD = TOD;
    proc->kernel_timer += cached_TOD - proc->tod_cache;
    proc->tod_cache = cached_TOD;
}

void reset_cached_tod(pcb_t* proc) {
    proc->tod_cache = TOD;
}

int get_proc_scheduler_index(pcb_t* p) {
    if (p==get_idle_proc()) return MAXPROC;
    else return get_process_index(p);
}





