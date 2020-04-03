#include <utils/utils.h>
#include "core/scheduler.h"

unsigned int clock_ticks_per_time_slice = 0;
struct list_head ready_queue;

pcb_t* running_proc;


void init_scheduler() {
    clock_ticks_per_time_slice = clock_ticks_per_period(SCHEDULER_TIME_SLICE);                      // This value will be the same until reboot or reset

    initPcbs();
    mkEmptyProcQ(&ready_queue);
}

unsigned int get_ticks_per_slice() {
    return clock_ticks_per_time_slice;
}


void time_slice_expired_callback() {

    // logica processi (cambia processo in esecuzione se necessario)

    // Increments the priority of each process in the ready_queue
//    struct pcb_t* target_proc;
//    list_for_each_entry(target_proc, &ready_queue, p_next) {
//        target_proc->priority += 1;
//    }
//
//    // Swap execution if the first ready process has a greater priority than the one executing
//    if (running_proc->priority < headProcQ(&ready_queue)->priority) {
//        running_proc->priority = running_proc->original_priority;       // Reset the previously running process' priority to the original
//        insertProcQ(&ready_queue, running_proc);
//
//        running_proc = removeProcQ(&ready_queue);
//    }
}


pcb_t* add_process(void* method, unsigned int priority, unsigned int vm_on, unsigned int km_on, unsigned int int_on) {

    pcb_t* p = allocPcb();
    if (p!=NULL) {

        set_pc(&p->p_s, method);

        p->priority = priority;
        p->original_priority = priority;


#ifdef TARGET_UMPS  // On umps we need to set the previous values since on process loading the vm, kernel and global interrupts stacks are popped

        if (km_on) {
            p->p_s.status = p->p_s.status & ~STATUS_KUp;                        // Kernel mode is on when the corresponding bit is 0
        }

        if (int_on) {
            p->p_s.status = p->p_s.status | STATUS_IEp;
            p->p_s.status = p->p_s.status | (1<<10);                            // TEMP For now just enable interval timer interrupts
        }

        if (vm_on) {
            p->p_s.status = p->p_s.status | STATUS_VMp;
        }

#elif TARGET_UARM                                       // On architectures without these mini-stacks (arm) we can just set the values normally
        set_virtual_mem(&p->p_s, vm_on);
        set_kernel_mode(&p->p_s, km_on);
        set_interrupts(&p->p_s, int_on);
#endif

        set_sp(&p->p_s, RAM_TOP - FRAME_SIZE*(get_process_index(p)+1));         // Use the index of the process as index of the frame, this should avoid overlaps at any time
                                                                                        // TODO does this needs a +1?
        insertProcQ(&ready_queue, p);                                                   // Insertion of the process in the ready_queue
        return p;

    } else {
        adderrbuf("ERROR: No free pcbs while adding a new process!");
        return NULL;
    }
}

void launch() {
    running_proc = removeProcQ(&ready_queue);
    set_interval_timer(get_ticks_per_slice());
    LDST(&running_proc->p_s);                                           // Launches the first process in the ready queue
}

pcb_t *get_running_proc() {
    return running_proc;
}



