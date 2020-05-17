#ifndef BIKAYA_DEVICES_H
#define BIKAYA_DEVICES_H

    #define DEVICE_ST_NOT_INSTALLED     0
    #define DEVICE_ST_READY             1
    #define DEVICE_ST_BUSY              3
    #define TERM_ST_DONE                5

    #define DEVICE_CMD_ACK              1
    #define DEVICE_CMD_FUNC_SPECIFIC    2

//
int* get_dev_sem(unsigned int line, unsigned int instance);

#endif //BIKAYA_DEVICES_H
