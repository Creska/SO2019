#include "core/exceptions/syscalls.h"
#include "core/processes/scheduler.h"
#include "core/exceptions/handler.h"
#include "utils/utils.h"
#include "utils/debug.h"
#include "devices/devices.h"
#include "core/processes/asl.h"
#include "core/exceptions/interrupts.h"
#include "core/system/system.h"








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
            save_syscall_return_register(interrupted_state, terminate_proc((pcb_t *) arg1));
            break;
        }

        case CREATEPROCESS: {
            save_syscall_return_register(interrupted_state,
                                         create_process((state_t *) arg1, (int) arg2, (pcb_t **) arg3));
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
            wait_io(arg1, (devreg_t*) arg2, (int) arg3);
            break;
        }

        case SPECPASSUP:
        {
            pcb_t * current_proc = get_running_proc();
            state_t** target_old_area = &(current_proc->spec_areas[arg1*2]);
            state_t** target_new_area = &current_proc->spec_areas[arg1*2+1];

            if ((*target_new_area)==NULL && (*target_old_area) == NULL) {
                DEBUG_LOG("The targeted spec areas weren't already set");
                *target_old_area = (state_t*)arg2;
                *target_new_area = (state_t*)arg3;

                // TEMP not sure about value assignment to those pointers, remember to check if values are not null in these logs
                DEBUG_LOG_PTR("The targeted old area is now set to ", current_proc->spec_areas[arg1*2]);
                DEBUG_LOG_PTR("The targeted new area is now set to ", current_proc->spec_areas[arg1*2]);

                // TODO need to implement "the other side of this", when those 3 handlers are called they need to check
                // if the corresponding spec pointers are set and load...

            } else {
                DEBUG_LOG("The targeted spec areas were already set, killing the callee");
                terminate_proc(current_proc);
            }
            break;
        }

        case GETPID: {
            // TODO check if this makes sense (test)
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
            adderrbuf("ERROR: Syscall not recognised");
            break;
        }
    }
}

