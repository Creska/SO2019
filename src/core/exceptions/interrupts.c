#include "core/exceptions/interrupts.h"


// Callback triggered when an external device raises an interrupt (meaning it is done with the current command or it has raised some error)
void done_io(enum ext_dev_type dev_type, unsigned int dev_n);


void consume_interrupts() {

#ifdef TARGET_UARM
    AREA(OLD, INT)->pc -= WORD_SIZE;     // On arm after an interrupt the pc needs to be decremented by one instruction (used ifdef to avoid useless complexity)
#endif

    // INTER-PROCESSOR INTERRUPTS
    //  if (is_interrupt_pending(0)) { }

    // PROCESSOR LOCAL TIMER INTERRUPTS (when supported)
    // if (is_interrupt_pending(1)) { }

    // INTERVAL TIMER INTERRUPT
    if (is_interrupt_pending(2)) {
        DEBUG_LOG("Interval timer interrupt pending");
        time_slice_callback();
    }

    // DEVICE INTERRUPTS ----------------------------------------------------------------------------------------------

    for (unsigned int line = N_IL-N_EXT_IL; line < IL_TERMINAL; ++line) {               // For each external device line (terminal excluded)
        if (is_interrupt_pending(line)) {
            DEBUG_LOG_UINT("Interrupt(s) pending on line ", line);
            unsigned int bitmap = *(unsigned*)CDEV_BITMAP_ADDR(line);
            for (int dev_num = 0; dev_num < N_DEV_PER_IL; ++dev_num) {                  // For each device on this line
                if (bitmap & 1) {                                                       // check the corresponding bit on the interrupt bitmap
                    done_io(get_ext_dev_type(line, 0), dev_num);       // if the bit is 1 there is an interrupt pending, call a done_io callback
                }
                bitmap = bitmap >> 1;
            }
        }
    }

    if (is_interrupt_pending(IL_TERMINAL)) {
        unsigned int bitmap = *(unsigned int*)CDEV_BITMAP_ADDR(IL_TERMINAL);
        for (unsigned int dev_num = 0; dev_num < N_DEV_PER_IL; ++dev_num) {
            if (bitmap & 1) {                                                                       // As above we check each terminal,
                DEBUG_LOG_INT("Interrupt pending for terminal ", dev_num);               // but this time we also need to check each subdevice
                devreg_t *dev_reg = (devreg_t*)DEV_REG_ADDR(IL_TERMINAL, dev_num);
                if ((get_status(TERM_RX, dev_reg) & DEV_STATUS_MASK) == TERM_ST_DONE) {
                    done_io(TERM_RX, dev_num);
                }
                if ((get_status(TERM_TX, dev_reg) & DEV_STATUS_MASK) == TERM_ST_DONE) {
                    done_io(TERM_TX, dev_num);
                }
            }
            bitmap = bitmap >> 1;
        }
    }
}


void wait_io(unsigned int command, devreg_t* dev_reg, int subdev) {
    // Retrieve device coordinates (calculating them from dev_reg address)
    unsigned int dev_line = GET_DEV_LINE((int) dev_reg);
    unsigned int dev_num = GET_DEV_INSTANCE((int) dev_reg);
    enum ext_dev_type dev_type = get_ext_dev_type(dev_line, subdev);
    dev_w_list* target_dev_list = get_dev_w_list(dev_type, dev_num);

    pcb_t* target_proc = pop_running();             // Set as running the first proc of the ready queue (and retrieve the previously running proc)
    target_dev_list->w_for_cmd_sem--;               // decrement the semaphore (passeren-like logic)
    if (target_dev_list->w_for_cmd_sem < 0) {
        // The device is already running, go to the semaphore queue
        insertBlocked(&target_dev_list->w_for_cmd_sem, target_proc);
        target_dev_list->w_for_cmd_sem++;
    } else {
        // The device is ready, send the command and go to w_for_res
        insertBlockedFifo(&target_dev_list->w_for_cmd_sem, target_proc);
        send_command(dev_type, command, dev_reg);
        target_dev_list->w_for_res = target_proc;
        target_proc->dev_w_list = &target_dev_list->w_for_res;
    }
    flush_kernel_time(target_proc);
}

// Callback triggered when a specific external device interrupt is raised, meaning that the given command
// is completed. The process that was waiting for a response is re-scheduled, if appropriate a new command is sent.
void done_io(enum ext_dev_type dev_type, unsigned int dev_n) {
    dev_w_list *target_dev_list = get_dev_w_list(dev_type, dev_n);

    if (target_dev_list->w_for_res!=NULL) {
        pcb_t* done_proc = removeBlocked(&target_dev_list->w_for_cmd_sem);
        devreg_t *dev_reg = (devreg_t*)DEV_REG_ADDR(get_ext_dev_line(dev_type), dev_n);
        if (done_proc==target_dev_list->w_for_res) {
            // The process wasn't terminated while waiting for response (since a termination would have removed it from the semaphore queue)
            done_proc->dev_w_list = NULL;
            schedule_proc(done_proc);

            unsigned int ret_status = get_status(dev_type, dev_reg);
            SYSCALL_RET_REG(&done_proc->p_s) = ret_status;
        }  // else the process was terminated in the meanwhile


        // Returns the device status through the syscall return register
        pcb_t* next_proc = headBlocked(&target_dev_list->w_for_cmd_sem);
        if (next_proc!=NULL) {
            DEBUG_LOG("The cmd waiting list isn't empty");
            unsigned int cmd_argument = SYSCALL_ARG1(&next_proc->p_s);
            send_command(dev_type, cmd_argument, dev_reg);
            target_dev_list->w_for_res = next_proc;
            next_proc->dev_w_list = &target_dev_list->w_for_res;
        } else {
            send_command(dev_type, DEVICE_CMD_ACK, dev_reg);
            target_dev_list->w_for_res = NULL;
            target_dev_list->w_for_cmd_sem++;
        }
    } else {
        addokbuf("A device interrupt was raised, but there's no process in the device queue, maybe you sent the "
                 "command directly to the device without the proper syscall?");
    }
}




