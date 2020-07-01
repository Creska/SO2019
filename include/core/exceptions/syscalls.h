#ifndef BIKAYA_SYSCALLS_H
#define BIKAYA_SYSCALLS_H

#include <core/processes/pcb.h>
#include "core/system/architecture.h"

// Syscall codes ------------------------------------------------------------------------------------------------------

// The maximum syscall number reserved by system syscalls
#define SYSTEM_SYSCALL_MAX      8

void consume_syscall(state_t *interrupted_state, pcb_t *interrupted_process);

#endif //BIKAYA_SYSCALLS_H
