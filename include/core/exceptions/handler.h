#ifndef BIKAYA_HANDLER_H
#define BIKAYA_HANDLER_H

#include "core/exceptions/syscalls.h"
#include "core/exceptions/interrupts.h"

// EXPOSED EXCEPTION HANDLER FUNCTIONS | called when the corresponding exception is raised ----------------------------

// Handler called when an interrupt (or multiple) is raised
void handle_interrupt();

// Handler called when a syscall/breakpoint exception is raised
void handle_sysbreak();

// Handler called when a TLB exception is raised
void handle_TLB();

// Handler called when a program trap exception is raised
void handle_trap();

#endif //BIKAYA_HANDLER_H
