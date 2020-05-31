#include "core/exceptions/interrupts.h"
#include "utils/debug.h"
#include "core/processes/scheduler.h"
#include "core/processes/asl.h"
#include "devices/devices.h"


void send_command(unsigned int line, unsigned int subdev, unsigned int command, devreg_t* dev_reg) {
    DEBUG_LOG("Sending command");
    DEBUG_LOG_INT("Line: ", GET_DEV_LINE((int)dev_reg));
    if  (line != IL_TERMINAL || subdev==1){
        dev_reg->dtp.command = command;
    } else {
        dev_reg->term.transm_command = command;
    }
}

unsigned int get_status(unsigned int line, unsigned int dev_num, unsigned int subdev) {
    devreg_t* dev_reg = (devreg_t*) DEV_REG_ADDR(line, dev_num);
    if (line != IL_TERMINAL || subdev==1) {
         return dev_reg->dtp.status;
    } else {
        return dev_reg->term.transm_status;
    }
}


void wait_io(unsigned int command, devreg_t* dev_reg, int subdev) {
    // Retrieve device coordinates (calculating them from dev_reg address)
    unsigned int dev_line = GET_DEV_LINE((int) dev_reg);
    unsigned int dev_num = GET_DEV_INSTANCE((int) dev_reg);

    dev_w_list* target_dev_list = get_dev_w_list(dev_line, dev_num, subdev);

    pcb_t* target_proc = swap_running();        // Set as running the first proc of the ready queue (and retrieve the previously running proc)

    DEBUG_LOG_INT("Sem:", target_dev_list->sem);
    DEBUG_LOG_UINT("Semaphore addr: ", (unsigned int)&target_dev_list->sem);
    target_dev_list->sem--;
    DEBUG_LOG("Pre if");
    if (target_dev_list->sem<0) {
        DEBUG_LOG("Busy semaphore");
        insertBlocked(&target_dev_list->sem, target_proc);
        target_proc->dev_command = command;     // TEMP
        target_dev_list->sem++;
    } else {
        // The device is ready, send the command and enqueue anyway
        insertBlockedFifo(&target_dev_list->sem, target_proc);
        send_command(dev_line, subdev, command, dev_reg);
        target_dev_list->w_for_res = target_proc;
    }
    DEBUG_LOG("Wait_io exit");
}

// Callback that needs to be called when a device interrupt is raised,
// meaning that the executing command is completed.
void done_io(unsigned int line, unsigned int dev_n, unsigned int subdev) {
    DEBUG_LOG("Done_io entry");
    dev_w_list *target_dev_list = get_dev_w_list(line, dev_n, subdev);

    pcb_t* done_proc = removeBlocked(&target_dev_list->sem);
    if (done_proc==target_dev_list->w_for_res) {
        // The process wasn't terminated ehile waiting for response
        schedule_proc(done_proc);
        unsigned int ret_status = get_status(line, dev_n, subdev);
        save_syscall_return_register(&done_proc->p_s, ret_status);
    }       // else the process was terminated in the meanwhile (don't do nothing ragarding its execution)


    // Returns the device status through the syscall return register
    devreg_t* dev_reg = (devreg_t*)DEV_REG_ADDR(line, dev_n);
    pcb_t* next_proc = headBlocked(&target_dev_list->sem);
    if (next_proc!=NULL) {
        DEBUG_LOG("The cmd waiting list isn't empty");
        send_command(line, subdev, next_proc->dev_command, dev_reg);
        target_dev_list->w_for_res = next_proc;
    } else {
        send_command(line, subdev, DEVICE_CMD_ACK, dev_reg);
        target_dev_list->w_for_res = NULL;
        target_dev_list->sem++;
    }
}

void consume_interrupts() {
    DEBUG_LOG("Consume interrupts entry");
    // Check and handle interrupts pending line by line in order of priority

     //INTER-PROCESSOR INTERRUPTS
//     if (is_interrupt_pending(0)) {
//         DEBUG_LOG("Inter processor interrupt");
//     }

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
            DEBUG_LOG_UINT("Found interrupt(s) pending on line ", line);
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
        DEBUG_LOG("Terminal interrupt pending");
        unsigned int bitmap = *(unsigned int*)CDEV_BITMAP_ADDR(7);
        DEBUG_LOG_BININT("Bitmap:", bitmap);
        unsigned int ya = 0;
        bitmap = bitmap >> 0;
        DEBUG_LOG_BININT("Bitmap:", bitmap);


        for (unsigned int dev_num = 0; dev_num < N_DEV_PER_IL; ++dev_num) {
            //DEBUG_LOG_INT("Device number ", dev_num);
            unsigned int shifted;
            if (dev_num!=0) {
                shifted = bitmap >> (unsigned int)dev_num;

            } else { shifted = bitmap; }
            unsigned int yo = shifted & 0x1;        // FIXME
            //DEBUG_LOG_BININT("Shifted: ", shifted);
            //DEBUG_LOG_BININT("Complete: ", yo);
            if (yo) {
                DEBUG_LOG_INT("Interrupt pending for terminal ", dev_num);

                unsigned int s0 = get_status(7, dev_num,0);
                DEBUG_LOG_UINT("Subdev 0 status: ", s0);
                if ((s0 & 0xff) == TERM_ST_DONE) {
                    DEBUG_LOG("Subdev 0 DONE");
                    done_io(7, dev_num, 0);
                }

                unsigned int s1 = get_status(7, dev_num,1);
                DEBUG_LOG_UINT("Subdev 1 status: ", s1);
                if ((s1 & 0xff) == TERM_ST_DONE) {
                    DEBUG_LOG("Subdev 1 DONE");
                    done_io(7, dev_num, 1);
                }
            }
        }
    }

    DEBUG_LOG("Consume interrupts exit");

}

