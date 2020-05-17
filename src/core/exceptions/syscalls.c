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
            /* LOOK AT ME -> È INUTILE MA LO LASCIO, NON SI SA MAI CHE POSSA TORNARE UTILE PER GLI HANDLER
            DEBUG_LOG_INT("SYS recognized: ", SPECPASSUP);
            state_t * interrupted_process_area = NULL;
            pcb_t * tmp_running_proc = get_running_proc();
            struct state_t * new_running_proc = &arg3;  //quì sbaglio, giusto?
            DEBUG_LOG("Switching between the correct handler...");

            switch (arg1)
            {

                case 0:
                {
                    DEBUG_LOG_INT("Recognized the sysbreak handler with code: ", arg1);
                    interrupted_process_area = get_old_area_sys_break();
                    DEBUG_LOG("get_old_area_sys_break : OK");
                    memcpy(&tmp_running_proc->p_s, interrupted_process_area, sizeof(state_t));
                    DEBUG_LOG("memcpy : OK");
                    handle_sysbreak();
                    set_kernel_mode(&new_running_proc, 1);
                    LDST(&new_running_proc);
                    DEBUG_LOG("LDST : OK");
                    break;
                }
                case 1:
                {
                    DEBUG_LOG_INT("Recognized the TLB handler with code: ", arg1);
                    interrupted_process_area = get_old_area_TLB();
                    memcpy(&tmp_running_proc->p_s, interrupted_process_area, sizeof(state_t));
                    LDST(&new_running_proc);
                    handle_TLB();
                    break;
                }
                case 2:
                {
                    interrupted_process_area = get_old_area_program_trap();
                    memcpy(&tmp_running_proc->p_s, interrupted_process_area, sizeof(state_t));
                    LDST(&new_running_proc);
                    handle_trap();
                    break;
                }
            }*/

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

//            switch (arg1)
//            {
//                /* TODO: check if the else statement is correct for the sys7's implementation
//                 * Info_0: The functions called in each else statement can be "called" just once if we write a waterfall-if instead of a switch
//                 * Info_1: How to get the buddy's element of the pcb_t list for the function outProcQ? headProcQ is not correct
//                 * Info_2: It's necessary to call both of the recursive_remove_proc and outProcQ functions?
//                 * Info_3: Remember always YO, YO && YO
//                 */
//
//                case 0:
//                {
//                    DEBUG_LOG_INT("Recognized the sysbreak handler with code: ", arg1);
//
//                    if (current_proc->old_area_sysbreak == NULL && current_proc->new_area_sysbreak == NULL)
//                    {
//                        DEBUG_LOG("Setting the old & new sysbreak's area given to the syscall inside the current process' pcb");
//                        current_proc->old_area_sysbreak = &arg2;
//                        current_proc->new_area_sysbreak = &arg3;
//                    }
//                    else
//                    {
//                        DEBUG_LOG("The process has already set the old & new sysbreak's area");
//                        DEBUG_LOG("I proced deleting the process and his childs...");
//                        recursive_remove_proc(current_proc);
//                        outProcQ(&(headProcQ(&current_proc)->p_next), current_proc);
//                    }
//                    break;
//                }
//                case 1:
//                {
//                    DEBUG_LOG_INT("Recognized the TLB handler with code: ", arg1);
//
//                    if (current_proc->old_area_TLB == NULL && current_proc->new_area_TLB == NULL)
//                    {
//                        DEBUG_LOG("Setting the old & new TLB's area given to the syscall inside the current process' pcb");
//                        current_proc->new_area_TLB = &arg3;
//                    }
//                    else
//                    {
//                        DEBUG_LOG("The process has already set the old & new sysbreak's area");
//                        DEBUG_LOG("I proced deleting the process and his childs...");
//                        recursive_remove_proc(current_proc);
//                        outProcQ(&(headProcQ(&current_proc)->p_next), current_proc);
//                    }
//                    break;
//                }
//                case 2:
//                {
//                    DEBUG_LOG_INT("Recognized the program trap handler with code: ", arg1);
//
//                    if (current_proc->old_area_progtrap == NULL && current_proc->new_area_progtrap == NULL)
//                    {
//                        DEBUG_LOG("Setting the old & new program trap's area given to the syscall inside the current process' pcb");
//                        current_proc->old_area_progtrap = &arg2;
//                        current_proc->new_area_progtrap = &arg3;
//                    }
//                    else
//                    {
//                        DEBUG_LOG("The process has already set the old & new sysbreak's area");
//                        DEBUG_LOG("I proced deleting the process and his childs...");
//                        recursive_remove_proc(current_proc);
//                        outProcQ(&(headProcQ(&current_proc)->p_next), current_proc);
//                    }
//                    break;
//                }
//            }
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

        //save dev.statuts
    }
}

