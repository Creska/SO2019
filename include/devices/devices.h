#ifndef BIKAYA_DEVICES_H
#define BIKAYA_DEVICES_H

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

    // All the available types of external devices
    enum ext_dev_type { DISK, TAPE, NETW, PRIN, TERM_TX, TERM_RX };


    // Returns the type of external device corresponding to the given line.
    //
    // In DEBUG mode panics if a non-existent or an internal dev line is given.
    enum ext_dev_type get_ext_dev_type(unsigned int line, unsigned int subdev);

    // Returns the actual system line corresponding to an external device type
    unsigned int get_ext_dev_line(enum ext_dev_type dev_type);

#endif //BIKAYA_DEVICES_H
