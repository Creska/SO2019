#include "core/exceptions/syscalls.h"
#include "core/processes/scheduler.h"
#include "utils/debug.h"
#include "core/exceptions/interrupts.h"
#include "core/system/system.h"



void consume_syscall(state_t *interrupted_state, pcb_t *interrupted_process) {

    unsigned int arg1 = SYSCALL_ARG1(interrupted_state),
                 arg2 = SYSCALL_ARG2(interrupted_state),
                 arg3 = SYSCALL_ARG3(interrupted_state);

    DEBUG_LOG_INT("Exception recognised as syscall number ", SYSCALL_N(interrupted_state));
    switch (SYSCALL_N(interrupted_state)) {                                                        // Using a switch since this will handle a few different syscalls in the next phases

        case GETCPUTIME: {
            unsigned int* user = (unsigned int*)arg1;
            unsigned int* kernel = (unsigned int*)arg2;
            unsigned int* wallclock = (unsigned int*)arg3;

            *user = interrupted_process->user_timer;
            *kernel = interrupted_process->kernel_timer + (TOD - interrupted_process->tod_cache);
            *wallclock = TOD - interrupted_process->tod_at_start;

            break;
        }

        case CREATEPROCESS: {
            SYSCALL_RET_REG(interrupted_state) = create_process((state_t *)arg1, (int)arg2, (pcb_t **)arg3);
            break;
        }

        case TERMINATEPROCESS: {
            SYSCALL_RET_REG(interrupted_state) = terminate_proc((pcb_t *)arg1);
            break;
        }

        case VERHOGEN: {
            v((int*)arg1);
            break;
        }

        case PASSEREN: {
            p((int *) arg1);
            break;
        }

        case WAITIO: {
            wait_io(arg1, (devreg_t*) arg2, (int) arg3);
            break;
        }

        case SPECPASSUP:
        {
            unsigned int type_code = arg1;              // Convert the type_code to the proper order (this facilitates area lookup on the long run)
            if (type_code==1) type_code = 2;
            else if (type_code==2) type_code = 1;
            DEBUG_LOG_INT("SPECPASSUP for type ", type_code);

            pcb_t * current_proc = get_running_proc();
            state_t** target_old_area = &(current_proc->spec_areas[type_code*2 + OLD]);
            state_t** target_new_area = &current_proc->spec_areas[type_code*2 + NEW];

            if ((*target_new_area)==NULL && (*target_old_area) == NULL) {
                DEBUG_LOG("The targeted spec areas weren't already set");
                *target_old_area = (state_t*)arg2;
                *target_new_area = (state_t*)arg3;
            } else {
                DEBUG_LOG("The targeted spec areas were already set, killing the callee");
                terminate_proc(current_proc);
            }
            break;
        }

        case GETPID: {
            pcb_t** pid = (pcb_t**) arg1;
            pcb_t** ppid = (pcb_t**) arg2;

            pcb_t* current_proc = get_running_proc();
            if (pid != NULL) {
                *pid = current_proc;
            }

            if (ppid != NULL) {
                *ppid = current_proc->p_parent;
            }
            break;
        }

        default: {
            adderrbuf("ERROR: Syscall not recognised. Did you mean syscall number 42?");
            break;
        }
    }
}