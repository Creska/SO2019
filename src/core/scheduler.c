#include <utils/utils.h>
#include "core/scheduler.h"

unsigned int clock_ticks_per_time_slice = 0;
struct list_head ready_queue;
pcb_t* running_proc = NULL;




void init_scheduler() {
    clock_ticks_per_time_slice = clock_ticks_per_period(SCHEDULER_TIME_SLICE);                      // This value will be the same until reboot or reset

    initPcbs();
    mkEmptyProcQ(&ready_queue);
}

unsigned int get_ticks_per_slice() {
    return clock_ticks_per_time_slice;
}


void on_scheduler_callback() {

    struct pcb_t* target_proc;
    list_for_each_entry(target_proc, &ready_queue, p_next) {                    // Increments the priority of each process in the ready_queue
        target_proc->priority += SCHED_PRIORITY_INC;
    }

    state_t* interrupted_process_state = get_old_area_int();

#ifdef TARGET_UARM
    interrupted_process_state->pc -= WORD_SIZE;                                       // On arm after an interrupt the pc needs to be decremented by one instruction (used ifdef to avoid useless complexity)
#endif

    mem_cpy(interrupted_process_state, &running_proc->p_s, sizeof(state_t));          // Copies the state_t saved in the old area in the pcb's state (otherwise data modified during execution would be lost)


    if (!list_empty(&ready_queue) && running_proc->priority <= headProcQ(&ready_queue)->priority) {             // Swap execution if the first ready process has a greater priority than the one executing (obviously if the ready queue is empty we don't need to swap)

        running_proc->priority = running_proc->original_priority;                                               // Reset the previously running process' priority to the original
        insertProcQ(&ready_queue, running_proc);                                                                // Insert the previously running process in the ready queue
        running_proc = removeProcQ(&ready_queue);                                                               // and set the first ready process as running (and remove it from the ready queue)
    }
}


pcb_t* add_process(void* method, unsigned int priority, unsigned int vm_on, unsigned int km_on, unsigned int int_timer_on, unsigned int other_int_on) {

    pcb_t* p = allocPcb();
    if (p!=NULL) {

        set_pc(&p->p_s, method);

        p->priority = priority;
        p->original_priority = priority;


#ifdef TARGET_UMPS  // On umps we need to set the previous values since on process loading the vm, kernel and global interrupts stacks are popped

        if (km_on) {
            p->p_s.status = p->p_s.status & ~STATUS_KUp;                        // Kernel mode is on when the corresponding bit is 0
        }

        if (int_timer_on) {
            p->p_s.status = p->p_s.status | STATUS_IEp;
            set_interval_timer_interrupts(&p->p_s, 1);
        }

        if (vm_on) {
            p->p_s.status = p->p_s.status | STATUS_VMp;
        }

#elif TARGET_UARM                                       // On architectures without these mini-stacks (arm) we can just set the values normally
        set_virtual_mem(&p->p_s, vm_on);
        set_kernel_mode(&p->p_s, km_on);
        set_interval_timer_interrupts(&p->p_s, int_timer_on);
        set_other_interrupts(&p->p_s, other_int_on);
#endif

        set_sp(&p->p_s, RAM_TOP - FRAME_SIZE*(get_process_index(p)+1));         // Use the index of the process as index of the frame, this should avoid overlaps at any time

        // FIXME the instant swap logic on higher priority is not working as expected
        if (running_proc && p->priority > running_proc->priority) {                 // Ensure swap if p has greater priority

            DEBUG_LOG("Starting instant process swapping");

            STST(&running_proc->p_s);                                               // (saving the state of the previously running process)

            state_t* temp = &running_proc->p_s;

            DEBUG_LOG("test");



            //set_interrupts(&get_running_proc()->p_s, 0);

            running_proc->priority = running_proc->original_priority;
            insertProcQ(&ready_queue, running_proc);
            running_proc = p;

            print_process_queue_priorities(&ready_queue);
            DEBUG_LOG("Resuming process after swap");

            // temp->pc_epc = getEPC();
            LDST(&running_proc->p_s);

        } else {
            insertProcQ(&ready_queue, p);                                                   // Insertion of the process in the ready_queue
        }



        return p;

    } else {
        adderrbuf("ERROR: No free pcbs while adding a new process!");
        return NULL;
    }
}

void launch() {
    if (!emptyProcQ(&ready_queue)) {
        running_proc = removeProcQ(&ready_queue);
        set_interval_timer(get_ticks_per_slice());
        LDST(&running_proc->p_s);
    } else {
        adderrbuf("Impossible to launch the system, no processes in the ready queue");
    }
}

pcb_t *get_running_proc() {
    return running_proc;
}


void recursive_remove_children(pcb_t* p) {

    pcb_t* first_child = removeChild(p);
    if (first_child != NULL) {
        outProcQ(&ready_queue, first_child);

        pcb_t* sib = nextSibling(first_child, first_child);
        while (sib!=NULL) {
            outProcQ(&ready_queue, sib);
            recursive_remove_children(sib);
            freePcb(sib);
            sib = nextSibling(sib, first_child);
        }
    }
}

void syscall3() {
    outProcQ(&ready_queue, running_proc);
    recursive_remove_children(running_proc);

    if (!emptyProcQ(&ready_queue)) {
        running_proc = removeProcQ(&ready_queue);
        LDST(&running_proc->p_s);
    } else {
        LOG ("No processes left");
        HALT();
    }
}



