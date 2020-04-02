#include "core/interrupts.h"

void consume_interrupts() {
    // Check and handle interrupts pending line by line in order of priority

    // INTER-PROCESSOR INTERRUPTS
    // if (is_interrupt_pending(0)) { }

    // PROCESSOR LOCAL TIMER INTERRUPTS
    // if (is_interrupt_pending(1)) { }

    // INTERVAL TIMER INTERRUPT
    if (is_interrupt_pending(2)) {
        time_slice_expired_callback();                      // Allows the scheduler to switch process execution if necessary
        set_interval_timer(get_ticks_per_slice());          // reset the interval timer (acknowledging the interrupt)
    }

    // DEVICE INTERRUPTS ----------------------------------------------------------------------------------------------

}
