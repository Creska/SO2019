#include <devices/terminal.h>
#include "core/scheduler.h"
#include "core/system.h"

unsigned int clock_ticks_per_time_slice;


void init_scheduler() {
    // This value will be the same until reboot or reset
    clock_ticks_per_time_slice = clock_ticks_per_period(SCHEDULER_TIME_SLICE);
    DEBUG_LOG_INT("Ticks: ", clock_ticks_per_time_slice);
    set_interval_timer(clock_ticks_per_time_slice);
}


void time_slice_expired_callback() {

    // logica processi

    set_interval_timer(clock_ticks_per_time_slice);
}


