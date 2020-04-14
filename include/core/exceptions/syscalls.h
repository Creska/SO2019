#ifndef BIKAYA_SYSCALLS_H
#define BIKAYA_SYSCALLS_H

#include "core/system/architecture.h"

// Syscall codes ------------------------------------------------------------------------------------------------------

// Syscall that terminates the running process and removes all its offspring from the ready queue
#define SYSCALL_TERMINATE_PROC      3

// Syscall that schedules a process for execution, causing it to be executed right away if it has an higher priority than the running process
// arg1: a pointer to a proc_init_data struct that defines all the relevant information for process initialization
#define SYSCALL_ADD_PROC            20



// Utility methods ----------------------------------------------------------------------------------------------------

// Loads the relevant registers from the given state t into the given ints
void load_syscall_registers(state_t* s, unsigned int* n, unsigned int* a1, unsigned int* a2, unsigned int* a3);


#endif //BIKAYA_SYSCALLS_H
