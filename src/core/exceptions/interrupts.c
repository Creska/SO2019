#include "core/exceptions/interrupts.h"
#include "utils/debug.h"
#include "core/processes/scheduler.h"
#include "core/processes/asl.h"
#include "devices/devices.h"


void send_command(unsigned int line, unsigned int subdev, unsigned int command, devreg_t* dev_reg) {
    if  (line != IL_TERMINAL){
        dev_reg->dtp.command = command;
    } else {
        if (subdev == 0) { dev_reg->term.transm_command = command; }
        else { dev_reg->term.recv_command = command; }
    }
}

unsigned int get_status(unsigned int line, unsigned int dev_num, unsigned int subdev) {
    devreg_t* dev_reg = (devreg_t*) DEV_REG_ADDR(line, dev_num);
    if (line != IL_TERMINAL) {
         return dev_reg->dtp.status;
    } else {
        if (subdev == 0) { return dev_reg->term.transm_status; }
        else { return dev_reg->term.recv_status; }
    }
}

void wait_io(unsigned int command, devreg_t* dev_reg, int subdev) {
    unsigned int dev_line = GET_DEV_LINE((int) dev_reg);
    unsigned int dev_num = GET_DEV_INSTANCE((int) dev_reg);

    DEBUG_LOG_UINT("dev line after SYS6's call: ", dev_line);
    DEBUG_LOG_UINT("dev num after SYS6's call: ", dev_num);

    int* target_semaphore = get_dev_sem(dev_line, dev_num);

    if (*target_semaphore) {
        // We can send the command now
        (*target_semaphore)--;
        send_command(dev_line, subdev, command, dev_reg);

    } else {
        // We can't send the command
        get_running_proc()->dev_command = command;
    }

    p_fifo(target_semaphore);
}

// Callback that needs to be called when a device interrupt is raised,
// meaning that the executing command is completed.
void done_io(unsigned int line, unsigned int dev_n, unsigned int subdev) {
    int* target_sem = get_dev_sem(line, dev_n);
    semd_t* target_semd = getSemd(target_sem);

    // TODO ACKNOWLEDGE
    pcb_t* target_pcb = headBlocked(target_sem);
    v_fifo(target_sem);          // Re-schedule the process that has completed its command


    unsigned int return_status = get_status(line, dev_n, subdev);
    save_syscall_return_register(&target_pcb->p_s, return_status);       // Put the device status in the syscall return register

    devreg_t* dev_reg = (devreg_t*) DEV_REG_ADDR(line, dev_n);
    if (list_empty(&target_semd->s_procQ)) {
        // No more commands waiting to be sent
        (*target_sem)++;
        send_command(line, subdev, DEVICE_CMD_ACK, dev_reg);
    } else {
        // There is still at least one command waiting to be sent to the device
        send_command(line, subdev, target_pcb->dev_command, dev_reg);
    }
}

void consume_interrupts() {
    // Check and handle interrupts pending line by line in order of priority

    // INTER-PROCESSOR INTERRUPTS
    // if (is_interrupt_pending(0)) { }

    // PROCESSOR LOCAL TIMER INTERRUPTS (for umps)
    // if (is_interrupt_pending(1)) { }

    // INTERVAL TIMER INTERRUPT
    if (is_interrupt_pending(2)) {
        DEBUG_LOG("Interval timer interrupt pending");
        time_slice_callback();                            // Allows the scheduler to switch process execution if necessary
    }

    // DEVICE INTERRUPTS ----------------------------------------------------------------------------------------------

    for (unsigned int line = 3; line < 7; ++line) {
        if (is_interrupt_pending(line)) {
            unsigned int bitmap = CDEV_BITMAP_ADDR(line);
            DEBUG_LOG_BININT("Bitmap:", bitmap);

            unsigned int dev_num = 0;
            while (dev_num < N_DEV_PER_IL) {
                if ((bitmap >> dev_num) & 1) {          // TODO check if valid logic (is this executed for the right device?)

                    done_io(line, dev_num, 0);
                }

                dev_num++;
            }
        }
    }


    if (is_interrupt_pending(7)) {
        unsigned int bitmap = CDEV_BITMAP_ADDR(7);
        DEBUG_LOG_BININT("Bitmap:", bitmap);

        unsigned int dev_num = 0;
        while (dev_num < N_DEV_PER_IL) {
            if ((bitmap >> dev_num) & 1) {          // TODO check if valid logic (is this executed for the right device?)

                if (get_status(7, dev_num, 0) == TERM_ST_DONE) {
                    done_io(7, dev_num, 0);
                }
                if (get_status(7, dev_num, 1) == TERM_ST_DONE) {
                    done_io(7, dev_num, 1);
                }
            }
            dev_num++;
        }
    }
}
