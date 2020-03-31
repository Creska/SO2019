#include "core/scheduler.h"
#include "pcb.h"

unsigned int clock_ticks_per_time_slice = 0;


void init_scheduler() {
    clock_ticks_per_time_slice = clock_ticks_per_period(SCHEDULER_TIME_SLICE);                      // This value will be the same until reboot or reset
    initPcbs();
}

unsigned int get_ticks_per_slice() {
    return clock_ticks_per_time_slice;
}


state_t* time_slice_expired_callback(state_t* interrupted_proc) {

    // logica processi


    return interrupted_proc;            // TEMP just return the interrupted process

}


