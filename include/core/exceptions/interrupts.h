#ifndef BIKAYA_INTERRUPTS_H
#define BIKAYA_INTERRUPTS_H

#include "core/system/system.h"

// Sends a command to the given external device, the currently running process is paused from execution until the device is done.
// If the device is busy enqueues the process on a dedicated semaphore until able to send the command.
void wait_io(unsigned int command, devreg_t* dev_reg, int subdev);


// Consumes the pending interrupts in order of priority
//
// This function is meant to be called by the interrupts handler when there's at least one unmasked interrupt pending
void consume_interrupts();

#endif //BIKAYA_HANDLER_H