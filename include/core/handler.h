#ifndef BIKAYA_PHASE0_HANDLER_H
#define BIKAYA_PHASE0_HANDLER_H


#include <core/interrupts.h>
#include <core/system.h>

// Handler called when an interrupt (or multiple) is raised
void handle_interrupt();

// Handler called when a syscall/breakpoint exception is raised
void handle_sysbreak();

void handle_TLB();

void handle_trap();

#endif //BIKAYA_PHASE0_HANDLER_H
