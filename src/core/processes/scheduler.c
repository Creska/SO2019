#include "utils/utils.h"
#include "core/processes/scheduler.h"
#include "core/system/system.h"
#include "utils/debug.h"
#include "core/processes/asl.h"


unsigned int clock_ticks_per_time_slice = 0;
struct list_head ready_queue;
pcb_t* running_proc = NULL;

pcb_t idle_proc;

// Semaphores for device waiting. One for each distinct device
dev_w_list dev_w_lists[N_EXT_IL + 1][N_DEV_PER_IL];


pcb_t* get_idle_proc() {
    return &idle_proc;
}

void idle() {
    while (1) {}
}


dev_w_list* get_dev_w_list(unsigned int line, unsigned int instance, unsigned int subdev) {
    unsigned int index = DEV_INDEX(line, subdev);
    return &dev_w_lists[index][instance];
}


void reset_int_timer() {
    DEBUG_LOG_INT("Resetting interval timer to ", clock_ticks_per_time_slice);
    set_interval_timer(clock_ticks_per_time_slice);
}

// Populates a pcb with the given data
//
// WARNING: the pcb is assumed to have all the fields initialized to 0/NULL (for example retrieved through get_free_pcb),
// if not might cause undefined behaviour since for efficiency this methods overrides some of the values only when target values are non-zero/non-NULL
void populate_pcb(pcb_t* p, proc_init_data* data) {

    set_pc(&p->p_s, data->method);
    set_sp(&p->p_s, RAM_TOP - FRAME_SIZE*(get_process_index(p)+1));                     // Use the index of the process as index of the frame, this should avoid overlaps at any time
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


// Retrieves a pcb from the freePCB list, panics if the freePCB list is empty, meaning that all the pcbs are already in use
pcb_t* get_free_pcb_else_panic() {
    pcb_t* p = allocPcb();
    if (p!=NULL) {
        return p;
    } else {
        adderrbuf("ERROR: The maximum number of processes has been reached!");
        return NULL;
    }
}


// This is meant to be called only one time during system initialization
void init_scheduler(proc_init_data starting_procs[], unsigned int procs_number, unsigned int time_slice) {

#ifdef DEBUG
    time_slice = time_slice*10;
#endif

    clock_ticks_per_time_slice = clock_ticks_per_period(time_slice);                      // This value will be the same until reboot or reset, we can just cache it

    initPcbs();
    initASL();
    mkEmptyProcQ(&ready_queue);

    for (int i = 0; i < procs_number; ++i) {
        pcb_t* p = get_free_pcb_else_panic();
        populate_pcb(p, &starting_procs[i]);
        insertProcQ(&ready_queue, p);
    }

    // Setting up the idle process
    reset_state(&idle_proc.p_s);
    proc_init_data idle_proc_data = {.km_on=1, .method=idle, .timer_int_on=1, .other_ints_on=1, .vm_on=0, .priority = 0};
    populate_pcb(&idle_proc, &idle_proc_data);

    set_sp(&idle_proc.p_s, RAM_TOP - FRAME_SIZE*(MAXPROC+1));      // TEMP maybe               // Use the index of the process as index of the frame, this should avoid overlaps at any time

    for (int l = 0; l < 6; ++l) {
        for (int d = 0; d < N_DEV_PER_IL; ++d) {
            dev_w_lists[l][d].sem = 1;
        }
    }
}



// Swaps the running process with a new one
void stop_running_proc() {
    if (running_proc!=&idle_proc) {
        running_proc->priority = running_proc->original_priority;                                               // Reset the previously running process' priority to the original
        insertProcQ(&ready_queue, running_proc);// Insert the previously running process in the ready queue
    } else {
        DEBUG_LOG("The running process was the IDLE PROC, stopping it without putting it in the ready queue");
    }
}

void set_running_proc(pcb_t* new_proc) {
    if (new_proc!=NULL) {
        running_proc = new_proc;
    } else {
        // TEMP
        DEBUG_LOG_UINT("Setting IDLE PROC as running process: ", get_process_index(&idle_proc));
        running_proc = &idle_proc;
    }
}



void launch() {
    if (!emptyProcQ(&ready_queue)) {
        set_running_proc(removeProcQ(&ready_queue));

        running_proc->tod_at_start = TOD;
        running_proc->tod_cache = TOD;


        DEBUG_LOG_INT("LAUNCHING PROCESS WITH PRIORITY: ", running_proc->priority);
        DEBUG_SPACING;

        reset_int_timer();
        LDST(&running_proc->p_s);
    } else {
        adderrbuf("Impossible to launch the system, no processes in the ready queue");
    }
}




void time_slice_callback() {

    struct pcb_t* target_proc;
    list_for_each_entry(target_proc, &ready_queue, p_next) {                    // Increments the priority of each process in the ready_queue (anti-starvation measure)
        DEBUG_LOG_INT("Increasing priority of process ", get_process_index(target_proc));
        target_proc->priority += PRIORITY_INC_PER_TIME_SLICE;
    }
#ifdef TARGET_UARM
    GET_AREA(OLD, INT)->pc -= WORD_SIZE;                                       // On arm after an interrupt the pc needs to be decremented by one instruction (used ifdef to avoid useless complexity)
#endif

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
}

// Schedules a new process for execution. If it has an higher priority than the running process
// it is executed immediately, otherwise it goes in the ready queue.
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
        memcpy(&p->p_s, s, sizeof(state_t));            // todo check
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





pcb_t *get_running_proc() {
    return running_proc;
}


// Removes the children of the given PCB.
int recursive_remove_proc(pcb_t* p) {

    pcb_t* to_be_freed = NULL;
    if (p->p_semkey!=NULL) {
        // The process is enqueued on a semaphore, remove it from the latter
        to_be_freed = outBlocked(p);
    } else {
        // The process is in the ready queue, remove it
        to_be_freed = outProcQ(&ready_queue, p);
    }

    if (to_be_freed!=NULL) {
        freePcb(to_be_freed);
    } else {
        adderrbuf("Qualcosa è andato storto");
        return -1;
    }

    pcb_t* target_child = removeChild(p);
    while (target_child!=NULL) {
        DEBUG_LOG_INT("Calling termination on child: ", get_process_index(target_child));
        if (recursive_remove_proc(target_child)) {
            return -1;                                                      // TODO return right away or try to continue? despite the error?
        }
        target_child = removeChild(p);
    }
    return 0;
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


void p(int* semaddr) {
    DEBUG_LOG_INT("P enter, semaphore has value ", *semaddr);
    DEBUG_LOG_UINT("Semaphore: ", semaddr);

    (*semaddr)--;

    if ((*semaddr)<0) {                                                       // If there are no available resources
        if (insertBlocked(semaddr, running_proc)) {                         // blocks the process on the semaphore
            adderrbuf("No free SEMDs");
        }
        pcb_t* new_proc = removeProcQ(&ready_queue);                          // Resume another process from the ready_queue
        if (new_proc!=NULL) {
            set_running_proc(new_proc);

        } else {
            // TODO idle dummy proc
            set_running_proc(&idle_proc);
//            adderrbuf("No process left after a PASSEREN call. Something must be wrong, "
//                      "every process is waiting on a semaphore, there's no ready process that can call a VERHOGEN.");
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

pcb_t *swap_running() {
    pcb_t* prev_running = running_proc;
    set_running_proc(removeProcQ(&ready_queue));

    DEBUG_LOG_UINT("Swapped proc: ", get_process_index(prev_running));
    DEBUG_LOG_UINT("...with proc: ", get_process_index(running_proc));
    return prev_running;
}



// Roba semafori
// TODO controllare che non venga contato il tempo in coda a un semaforo per CPU time
// TODO rimozione di un processo dalla coda di un semaforo con terminate process





