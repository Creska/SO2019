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

    p(target_semaphore);
}

// Callback that needs to be called when a device interrupt is raised,
// meaning that the executing command is completed.
void done_io(unsigned int line, unsigned int dev_n) {
    int* target_sem = get_dev_sem(line, dev_n);
    semd_t* target_semd = getSemd(target_sem);

    // TODO ACKNOWLEDGE

    v(target_sem);          // Re-schedule the process that has completed its command
    //TODO Put the device status in the syscall return register

    if (list_empty(&target_semd->s_procQ)) {
        // No more commands waiting to be sent
        (*target_sem)++;
    } else {
        // There is still at least one command waiting to be sent to the device
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

    if (is_interrupt_pending(3)) {
        DEBUG_LOG("Disk device interrupt pending");

    }

    if (is_interrupt_pending(4)) {

        DEBUG_LOG("Tape device interrupt pending");
    }

    if (is_interrupt_pending(5)) {
        DEBUG_LOG("Network device interrupt pending");
    }

    if (is_interrupt_pending(6)) {
        DEBUG_LOG("Printer device interrupt pending");
    }

    if (is_interrupt_pending(7)) {

        DEBUG_LOG("Terminal device interrupt pending");


        unsigned int bitmap = CDEV_BITMAP_ADDR(7);
        DEBUG_LOG_BININT("Bitmap:", bitmap);

        unsigned int dev_num = 0;
        while (dev_num < N_DEV_PER_IL) {
            if ((bitmap >> dev_num) & 1) {          // TODO check if valid logic (is this executed for the right device?)

                done_io(7, dev_num);
            }

            dev_num++;
        }




    }
}
