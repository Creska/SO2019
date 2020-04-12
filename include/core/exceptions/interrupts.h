#ifndef BIKAYA_PHASE0_INTERRUPTS_H
#define BIKAYA_PHASE0_INTERRUPTS_H

// Consumes the pending interrupts in order of priority
//
// This function is meant to be called by the interrupts handler when there's at least one unmasked interrupt pending
void consume_interrupts();

#endif //BIKAYA_PHASE0_HANDLER_H