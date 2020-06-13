#include "core/exceptions/interrupts.h"
#include "utils/debug.h"
#include "core/processes/scheduler.h"
#include "core/processes/asl.h"
#include "devices/devices.h"


void send_command(enum ext_dev_type ext_dev, unsigned int command, devreg_t* dev_reg) {
    if  (ext_dev != TERM_TX){
        dev_reg->dtp.command = command;
    } else {
        dev_reg->term.transm_command = command;
    }
}

unsigned int get_status(enum ext_dev_type dev_type, devreg_t* dev_reg) {
    if (dev_type!=TERM_TX) {
         return dev_reg->dtp.status;
    } else {
        return dev_reg->term.transm_status;
    }
}

void wait_io(unsigned int command, devreg_t* dev_reg, int subdev) {
    // Retrieve device coordinates (calculating them from dev_reg address)
    unsigned int dev_line = GET_DEV_LINE((int) dev_reg);
    unsigned int dev_num = GET_DEV_INSTANCE((int) dev_reg);
    enum ext_dev_type dev_type = get_ext_dev_type(dev_line, subdev);
    dev_w_list* target_dev_list = get_dev_w_list(dev_type, dev_num);

    pcb_t* target_proc = swap_running();        // Set as running the first proc of the ready queue (and retrieve the previously running proc)


    target_dev_list->sem--;
    if (target_dev_list->sem<0) {
        insertBlocked(&target_dev_list->sem, target_proc);
        target_proc->dev_command = command;                     // TEMP
        target_dev_list->sem++;
    } else {
        // The device is ready, send the command and enqueue anyway
        insertBlockedFifo(&target_dev_list->sem, target_proc);
        send_command(dev_type, command, dev_reg);
        target_dev_list->w_for_res = target_proc;
    }
    DEBUG_LOG("Wait_io exit");
}

// Callback that needs to be called when a device interrupt is raised,
// meaning that the executing command is completed.
void done_io(enum ext_dev_type dev_type, unsigned int dev_n) {
    DEBUG_LOG("Done_io entry");
    dev_w_list *target_dev_list = get_dev_w_list(dev_type, dev_n);

    pcb_t* done_proc = removeBlocked(&target_dev_list->sem);
    if (done_proc==target_dev_list->w_for_res) {
        // The process wasn't terminated while waiting for response
        schedule_proc(done_proc);

        devreg_t *dev_reg = (devreg_t*)DEV_REG_ADDR(get_ext_dev_line(dev_type), dev_n);
        unsigned int ret_status = get_status(dev_type, dev_reg);
        save_syscall_return_register(&done_proc->p_s, ret_status);
    }       // else the process was terminated in the meanwhile (don't do nothing ragarding its execution)


    // Returns the device status through the syscall return register
    devreg_t* dev_reg = (devreg_t*)DEV_REG_ADDR(get_ext_dev_line(dev_type), dev_n);
    pcb_t* next_proc = headBlocked(&target_dev_list->sem);
    if (next_proc!=NULL) {
        DEBUG_LOG("The cmd waiting list isn't empty");
        send_command(dev_type, next_proc->dev_command, dev_reg);
        target_dev_list->w_for_res = next_proc;
    } else {
        send_command(dev_type, DEVICE_CMD_ACK, dev_reg);
        target_dev_list->w_for_res = NULL;
        target_dev_list->sem++;
    }
}

void consume_interrupts() {

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

    for (unsigned int line = N_IL-N_EXT_IL; line < IL_TERMINAL; ++line) {
        if (is_interrupt_pending(line)) {
            DEBUG_LOG_UINT("Interrupt(s) pending on line ", line);
            unsigned int bitmap = *(unsigned*)CDEV_BITMAP_ADDR(line);
            for (int dev_num = 0; dev_num < N_DEV_PER_IL; ++dev_num) {                  // For each device on this line
                if (bitmap & 1) {                                                       // check the corresponding bit on the interrupt bitmap
                    done_io(get_ext_dev_type(line, 0), dev_num);
                }
                bitmap = bitmap >> 1;
            }
        }
    }


    if (is_interrupt_pending(IL_TERMINAL)) {
        unsigned int bitmap = *(unsigned int*)CDEV_BITMAP_ADDR(7);
        for (unsigned int dev_num = 0; dev_num < N_DEV_PER_IL; ++dev_num) {
            if (bitmap & 1) {
                DEBUG_LOG_INT("Interrupt pending for terminal ", dev_num);
                devreg_t *dev_reg = (devreg_t*)DEV_REG_ADDR(7, dev_num);

                if ((get_status(TERM_RX, dev_reg) & 0xff) == TERM_ST_DONE) done_io(TERM_RX, dev_num);
                if ((get_status(TERM_TX, dev_reg) & 0xff) == TERM_ST_DONE) done_io(TERM_TX, dev_num);
            }
            bitmap = bitmap >> 1;
        }
    }
    DEBUG_LOG("Consume interrupts exit");
}

