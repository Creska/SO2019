#include "core/scheduler.h"

unsigned int clock_ticks_per_time_slice = 0;

struct list_head ready_queue;


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

}


void add_process(void* method, unsigned int priority, unsigned int vm_on, unsigned int km_on, unsigned int int_on) {

    pcb_t* p = allocPcb();
    if (p!=NULL) {

        set_pc(&p->p_s, method);

        p->priority = priority;
        p->original_priority = priority;

        set_virtual_mem(&p->p_s, vm_on);
        set_kernel_mode(&p->p_s, km_on);
        set_interrupts(&p->p_s, int_on);

        set_sp(&p->p_s, RAM_TOP - FRAMESIZE*get_process_index(p));      // Use the index of the process as index of the frame, this should avoid overlaps for different processes

        insertProcQ(&ready_queue, p);                                           // Insertion of the process in the ready_queue

        DEBUG_LOG_INT("Process index: ", get_process_index(p));

    } else {
        adderrbuf("No free pcbs while adding a new process");
    }
}

void run() {
    LDST(&headProcQ(&ready_queue)->p_s);
}



