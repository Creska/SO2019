#include "core/exceptions/interrupts.h"
#include "utils/debug.h"
#include "core/system/system.h"
#include "core/processes/scheduler.h"


void consume_interrupts() {
    // Check and handle interrupts pending line by line in order of priority

    // INTER-PROCESSOR INTERRUPTS
    // if (is_interrupt_pending(0)) { }

    // PROCESSOR LOCAL TIMER INTERRUPTS (for umps)
    // if (is_interrupt_pending(1)) {  }

    // INTERVAL TIMER INTERRUPT
    if (is_interrupt_pending(2)) {
        DEBUG_LOG("Interval timer interrupt pending");
        time_slice_callback();                            // Allows the scheduler to switch process execution if necessary
    }

    // DEVICE INTERRUPTS ----------------------------------------------------------------------------------------------
    // ...
    // here we could already handle terminal interrupts, but the test processes and our terminal functionality handle it themselves, so it's not needed for now

}
