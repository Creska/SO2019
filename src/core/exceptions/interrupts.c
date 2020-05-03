#include "core/exceptions/interrupts.h"
#include "utils/debug.h"
#include "core/system/system.h"
#include "core/processes/scheduler.h"
#include "core/processes/asl.h"


typedef struct semdev {
    semd_t disk[DEV_PER_INT];
    semd_t tape[DEV_PER_INT];
    semd_t network[DEV_PER_INT];
    semd_t printer[DEV_PER_INT];
    semd_t terminalR[DEV_PER_INT];
    semd_t terminalT[DEV_PER_INT];
} semdev;


void consume_interrupts() {
    // Check and handle interrupts pending line by line in order of priority

    // INTER-PROCESSOR INTERRUPTS
    // if (is_interrupt_pending(0)) { }

    // PROCESSOR LOCAL TIMER INTERRUPTS (for umps)
    // if (is_interrupt_pending(1)) { }

    // INTERVAL TIMER INTERRUPT
    if (is_interrupt_pending(2)) {
        DEBUG_LOG("Interval timer interrupt pending");
        time_slice_callback();                            // Allows the scheduler to switch process execution if necessary
    }

    // DEVICE INTERRUPTS ----------------------------------------------------------------------------------------------
    // ...
    // here we could already handle terminal interrupts, but the test processes and our terminal functionality handle it themselves, so it's not needed for now

    if (is_interrupt_pending(3)) {
        DEBUG_LOG("Disk device interrupt pending");

    }

    if (is_interrupt_pending(4)) {
        DEBUG_LOG("Tape device interrupt pending");
    }

    if (is_interrupt_pending(5)) {
        DEBUG_LOG("Network device interrupt pending");
    }

    if (is_interrupt_pending(6)) {
        DEBUG_LOG("Printer device interrupt pending");
    }

    if (is_interrupt_pending(7)) {
        DEBUG_LOG_BININT("Bitmap:", CDEV_BITMAP_ADDR(7));
        DEBUG_LOG("Terminal device interrupt pending");
    }
}
