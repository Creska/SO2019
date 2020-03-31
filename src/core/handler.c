#include <core/system.h>
#include "core/scheduler.h"


void handle_interrupt() {
    state_t* interrupted_process = get_old_area_int();
    state_t* process_to_resume = time_slice_expired_callback(interrupted_process);          // Allows the scheduler to switch process execution if necessary

    set_interval_timer(get_ticks_per_slice());
    LDST(process_to_resume);                                 // Resume the execution of the process

    // TODO check that the process is resumed with the pc at the right address (there could be an offset of a few instructions)
}