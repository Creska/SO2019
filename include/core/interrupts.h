#ifndef BIKAYA_PHASE0_INTERRUPTS_H
#define BIKAYA_PHASE0_INTERRUPTS_H

#include "core/system.h"
#include "core/scheduler.h"

// Consumes the pending interrupts in order of priority
void consume_interrupts();

#endif //BIKAYA_PHASE0_HANDLER_H