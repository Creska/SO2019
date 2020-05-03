#include "core/exceptions/syscalls.h"
#include "core/processes/scheduler.h"
#include "utils/debug.h"


void load_syscall_registers(state_t* s, unsigned int* n, unsigned int* a1, unsigned int* a2, unsigned int* a3) {
#ifdef TARGET_UMPS                                  // (handled with ifdef for now to avoid useless complexity, in the next phase this kind of stuff could be handled with a pattern similar to system.h)
    *n = s->reg_a0;
    *a1 = s->reg_a1;
    *a2 = s->reg_a2;
    *a3 = s->reg_a3;
#elif TARGET_UARM
    *n = s->a1;
    *a1 = s->a2;
    *a2 = s->a3;
    *a3 = s->a4;
#endif
}

void save_return_register(state_t *s, unsigned int return_val) {
#ifdef TARGET_UMPS
    s->reg_v0 = return_val;
#elif TARGET_UARM
    s->a1 = return_val;
#endif
}

void consume_syscall(state_t *interrupted_state, pcb_t *interrupted_process) {

    unsigned int sys_n, arg1, arg2, arg3;                       // Retrieving syscall number and arguments from processor registers
    load_syscall_registers(interrupted_state, &sys_n, &arg1, &arg2, &arg3);

    DEBUG_LOG_INT("Exception recognised as syscall number ", sys_n);
    switch (sys_n) {                                                        // Using a switch since this will handle a few different syscalls in the next phases

        case GETCPUTIME: {

            unsigned int* user = (unsigned int*)arg1;
            unsigned int* kernel = (unsigned int*)arg2;
            unsigned int* wallclock = (unsigned int*)arg3;

            *user = interrupted_process->user_timer;
            *kernel = interrupted_process->kernel_timer + (TOD - interrupted_process->tod_cache);
            *wallclock = TOD - interrupted_process->tod_at_start;

            break;
        }

        case TERMINATEPROCESS: {
            save_return_register(interrupted_state, terminate_proc((pcb_t*)arg1));
            break;
        }

        case CREATEPROCESS: {
            save_return_register(interrupted_state, create_process((state_t*)arg1, (int)arg2, (pcb_t**)arg3));
            break;
        }

        case VERHOGEN: {
            int* semaddr = (int*)arg1;
            v(semaddr);
            break;
        }

        case PASSEREN: {
            int* semaddr = (int*)arg1;
            p(semaddr);
            break;
        }

        case WAITIO: {
            adderrbuf("Syscall WAIT_IO not implemented (yet)");
            unsigned int command = arg1;
            unsigned int* reg= (unsigned int *) arg2;
            int subdev = (int) arg3;

            *reg = command;
            break;
        }

        case SPECPASSUP: {
            adderrbuf("Syscall SPECPASSUP not implemented (yet)");
            break;
        }

        case GETPID: {
            adderrbuf("Syscall GETPID not implemented (yet)");
            break;
        }

        default: {
            adderrbuf("ERROR: Syscall not implemented");
            break;
        }
    }
}

