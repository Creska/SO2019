#ifndef BIKAYA_DEVICES_H
#define BIKAYA_DEVICES_H

    #include "core/system/system.h"
    #include "utils/debug.h"


// All the available types of external devices
    enum ext_dev_type { DISK, TAPE, NETW, PRIN, TERM_TX, TERM_RX };

    // Device states
    #define DEVICE_ST_NOT_INSTALLED     0
    #define DEVICE_ST_READY             1
    #define DEVICE_ST_BUSY              3
    #define TERM_ST_DONE                5

    // Device commands
    #define DEVICE_CMD_ACK              1
    #define DEVICE_CMD_FUNC_SPECIFIC    2

    // Extra device's number
    #define EXTRA_DEV_NUM  1

    // Returns the device's instance
    #define GET_DEV_INSTANCE(reg) ((reg - DEV_REG_START)/(DEV_REG_SIZE)) % N_DEV_PER_IL

    // Returns the device line's number from it's device register address
    #define GET_DEV_LINE(reg) ((reg - DEV_REG_START)/DEV_REG_SIZE)/N_DEV_PER_IL + GET_DEV_START


    // Returns the type of external device corresponding to the given line (and subdevice).
    //
    // In DEBUG mode panics if a non-existent or an internal dev line is given.
    enum ext_dev_type get_ext_dev_type(unsigned int line, unsigned int subdev);

    // Returns the actual system line corresponding to an external device type
    unsigned int get_ext_dev_line(enum ext_dev_type dev_type);

    // Sends a command to the given external device
    void send_command(enum ext_dev_type ext_dev, unsigned int command, devreg_t* dev_reg);

    // Returns the status of the given external device
    unsigned int get_status(enum ext_dev_type dev_type, devreg_t* dev_reg);

#endif //BIKAYA_DEVICES_H
