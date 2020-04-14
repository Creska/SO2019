#include "utils/utils.h"
#include "core/processes/scheduler.h"
#include "utils/debug.h"

unsigned int clock_ticks_per_time_slice = 0;
struct list_head ready_queue;
pcb_t* running_proc = NULL;


void reset_int_timer() {
    set_interval_timer(clock_ticks_per_time_slice);
}

void populate_pcb(pcb_t* p, proc_init_data* data) {

    set_pc(&p->p_s, data->method);
    set_sp(&p->p_s, RAM_TOP - FRAME_SIZE*(get_process_index(p)+1));                     // Use the index of the process as index of the frame, this should avoid overlaps at any time
    set_interval_timer_interrupts(&p->p_s, data->timer_int_on);
    set_other_interrupts(&p->p_s, data->other_ints_on);
    p->priority = data->priority;
    p->original_priority = data->priority;

#ifdef TARGET_UMPS  // On umps we need to set the previous values since on process loading the vm, kernel and global interrupts stacks are popped
    set_virtual_mem(&p->p_s, 0);
    if (!data->km_on) { p->p_s.status = p->p_s.status | STATUS_KUp; }                                 // Kernel mode is on when the corresponding bit is 0
    if (data->vm_on) { p->p_s.status = p->p_s.status | STATUS_VMp; }
    if (data->timer_int_on || data->other_ints_on) { p->p_s.status = p->p_s.status | STATUS_IEp; }           // Manually sets the previous global interrupt switch
#elif TARGET_UARM  // On architectures without these mini-stacks (arm in our case) we can just set the values normally (using system.h methods)
    set_virtual_mem(&p->p_s, data->vm_on);
    set_kernel_mode(&p->p_s, data->km_on);
#endif
}

pcb_t* get_free_pcb() {
    pcb_t* p = allocPcb();
    if (p!=NULL) {
        return p;
    } else {
        adderrbuf("ERROR: The maximum number of processes has been reached!");
        return NULL;
    }
}

// This is meant to be called only one time during system initialization
void init_scheduler(proc_init_data starting_procs[], unsigned int procs_number) {
    clock_ticks_per_time_slice = clock_ticks_per_period(SCHEDULER_TIME_SLICE);                      // This value will be the same until reboot or reset, we can just cache it

    initPcbs();
    mkEmptyProcQ(&ready_queue);

    for (int i = 0; i < procs_number; ++i) {
        pcb_t* p = get_free_pcb();
        populate_pcb(p, &starting_procs[i]);
        insertProcQ(&ready_queue, p);
    }
}

void launch() {
    if (!emptyProcQ(&ready_queue)) {
        running_proc = removeProcQ(&ready_queue);
        reset_int_timer();

        DEBUG_LOG_INT("LAUNCHING PROCESS WITH PRIORITY: ", running_proc->priority);
        DEBUG_SPACING;

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
        memcpy(&running_proc->p_s, interrupted_process_state, sizeof(state_t));                                 // Copies the state_t saved in the old area in the pcb's state (otherwise data modified during execution would be lost)
        running_proc->priority = running_proc->original_priority;                                               // Reset the previously running process' priority to the original
        insertProcQ(&ready_queue, running_proc);                                                                // Insert the previously running process in the ready queue
        running_proc = removeProcQ(&ready_queue);                                                               // and set the first ready process as running (and remove it from the ready queue)
    } else {
        DEBUG_LOG("The current process still has the higher priority, resuming its execution");
    }
}


pcb_t* add_process(proc_init_data* data) {

    DEBUG_LOG_INT("ADDING NEW PROCESS WITH PRIORITY: ", data->priority);

    pcb_t* p = get_free_pcb();
    populate_pcb(p, data);


    if (p->priority > running_proc->priority) {                                     // Ensure instant process swap if p has greater priority than the running process
        DEBUG_LOG("The newly added process has higher priority than the running one, swapping them instantly");

        memcpy(&running_proc->p_s, get_old_area_sys_break(), sizeof(state_t));      // Copy the processor state saved in the old area to the soon-to-be-suspended process

        running_proc->priority = running_proc->original_priority;                   // resetting the priority to the original priority on ready queue insertion
        insertProcQ(&ready_queue, running_proc);
        running_proc = p;
        reset_int_timer();                                                          // Since a different process will be resumed we give it a full time-slice

    } else {
        DEBUG_LOG ("Resuming running process after new process addition to the ready queue\n");
        insertProcQ(&ready_queue, p);
    }
    return p;
}



pcb_t *get_running_proc() {
    return running_proc;
}


void recursive_remove_children(pcb_t* p) {              // TODO this might need some debugging, might as well do it when we implement parental relationship between processes

    pcb_t* child = removeChild(p);
    while (child != NULL) {                             // As long as there is a first child remove it from the tree structure, from the ready queue
        recursive_remove_children(child);               // and free it with DFS recursion
        outProcQ(&ready_queue, child);
        freePcb(child);
        child = removeChild(p);
    }
}


void terminate_running_proc() {
    recursive_remove_children(running_proc);

    if (!emptyProcQ(&ready_queue)) {
        running_proc = removeProcQ(&ready_queue);
    } else {
        addokbuf("No processes left after the last process termination\n");
        HALT();
    }
}