#include "utils/utils.h"
#include "core/processes/scheduler.h"
#include "core/system/system.h"
#include "utils/debug.h"
#include "core/processes/asl.h"


unsigned int clock_ticks_per_time_slice = 0;
struct list_head ready_queue;
pcb_t* running_proc = NULL;


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
}

// Swaps the running process with a new one
void stop_running_proc() {
    running_proc->priority = running_proc->original_priority;                                               // Reset the previously running process' priority to the original
    insertProcQ(&ready_queue, running_proc);// Insert the previously running process in the ready queue

}

void set_running_proc(pcb_t* new_proc) {
    running_proc = new_proc;
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
        target_proc->priority += PRIORITY_INC_PER_TIME_SLICE;
    }

    state_t* interrupted_process_state = get_old_area_int();

#ifdef TARGET_UARM
    interrupted_process_state->pc -= WORD_SIZE;                                       // On arm after an interrupt the pc needs to be decremented by one instruction (used ifdef to avoid useless complexity)
#endif

    if (!list_empty(&ready_queue) && running_proc->priority <= headProcQ(&ready_queue)->priority) {             // Swap execution if the first ready process has a greater priority than the one executing (obviously if the ready queue is empty we don't need to swap)
        DEBUG_LOG_INT("Swapping to processes with original priority: ", headProcQ(&ready_queue)->original_priority);

        stop_running_proc(interrupted_process_state);// and set the first ready process as running (and remove it from the ready queue)
        set_running_proc(removeProcQ(&ready_queue));
    } else {
        DEBUG_LOG("The current process still has the higher priority, resuming its execution");
    }
}

//
//pcb_t* add_process(proc_init_data* data) {              // TODO set tod and remember to set timers on pcb reset
//
//    DEBUG_LOG_INT("ADDING NEW PROCESS WITH PRIORITY: ", data->priority);
//
//    pcb_t* p = get_free_pcb();
//    populate_pcb(p, data);
//
//
//    if (p->priority > running_proc->priority) {                                     // Ensure instant process swap if p has greater priority than the running process
//        DEBUG_LOG("The newly added process has higher priority than the running one, swapping them instantly");
//
//        stop_running_proc(get_old_area_sys_break());
//        set_running_proc(p);
//
//        reset_int_timer();                                                          // Since a different process will be resumed we give it a full time-slice
//
//    } else {
//        DEBUG_LOG ("Resuming running process after new process addition to the ready queue\n");
//        insertProcQ(&ready_queue, p);
//    }
//    return p;
//}

int create_process(state_t *s, int priority, pcb_t **cpid) {
    pcb_t* p = allocPcb();
    if (p!=NULL) {
        memcpy(&p->p_s, s, sizeof(state_t));            // todo check
        p->priority = priority;
        p->original_priority = priority;
        insertChild(running_proc, p);               // Set the new process as child of the running one
        *cpid = p;

        if (priority > running_proc->priority) {
            DEBUG_LOG("The newly created process has higher priority than the running one, swapping them instantly");
            stop_running_proc();
            set_running_proc(p);
            reset_int_timer();
        } else {
            DEBUG_LOG ("The newly created process has lower priority than the running one, just inserting it in the ready queue");
            insertProcQ(&ready_queue, p);
        }
        return 0;
    } else {
        DEBUG_LOG("Maximum number of processes was reached, failed to create a new one");
        return -1;
    }
}


pcb_t *get_running_proc() {
    return running_proc;
}


//void recursive_remove_children(pcb_t* p) {              // TODO this might need some debugging, might as well do it when we implement parental relationship between processes
//
//    pcb_t* child = removeChild(p);
//    while (child != NULL) {                             // As long as there is a first child remove it from the tree structure, from the ready queue
//        recursive_remove_children(child);               // and free it with DFS recursion
//        outProcQ(&ready_queue, child);
//        freePcb(child);
//        child = removeChild(p);
//    }
//}
//
//
//void terminate_running_proc() {
//    recursive_remove_children(running_proc);
//    if (!emptyProcQ(&ready_queue)) {
//        set_running_proc(removeProcQ(&ready_queue));
//        reset_int_timer();
//    } else {
//        addokbuf("No processes left after the last process termination\n");
//        HALT();
//    }
//}


// Removes the children of the given PCB.
int recursive_remove_proc_children(pcb_t* p) {                          // TODO test this functionality with actual process trees

    DEBUG_LOG("Recursive trermination entry point");
    pcb_t* to_be_freed = outProcQ(&ready_queue, p);                     // Remove p from the process queue and free it
    if (to_be_freed!=NULL) {
        freePcb(to_be_freed);
    } else {
        DEBUG_LOG("Qualcosa è andato storto");
        return -1;                                                      // TODO return right away or try to continue? despite the error?
    }

    pcb_t* target_child = outChild(p);
    while (target_child!=NULL) {
        if (recursive_remove_proc_children(target_child)) {
            return -1;
        }

        target_child = outChild(p);

    }
    return 0;
}


int terminate_proc(pcb_t *p) {
    DEBUG_LOG("Termination function entry point");
    if (p==NULL) { p = running_proc; }

    insertProcQ(&ready_queue, running_proc);                                        // Insert temporarily the running proc in the ready queue to facilitate recursion and removal checks
    int ret = recursive_remove_proc_children(p);

    pcb_t* retrieved_running_proc = outProcQ(&ready_queue, running_proc);           // Take back the running process from the ready queue
    if (retrieved_running_proc==NULL) {                                             // If the running process wasn't in the ready queue at this point it means that it was removed
        pcb_t* new_proc = headProcQ(&ready_queue);
        if (new_proc!=NULL) {
            set_running_proc(headProcQ(&ready_queue));                              // so we run a new process if the ready queue isn't empty
        } else {
            addokbuf("No processes left after the last process termination\n");
            HALT();
        }
    }
    return ret;
}

void p(int* semaddr) {
    semaddr--;                  // TODO check initial semaphore status


    if (semaddr<0) {
        if (insertBlocked(semaddr, running_proc)) {
            adderrbuf("No free SEMDs");
        }
        pcb_t* new_proc = headProcQ(&ready_queue);
        if (new_proc!=NULL) {
            set_running_proc(new_proc);
        } else {
            adderrbuf("No process left");
        }
        semaddr++;
    }



}

void v(int* semaddr) {

}





