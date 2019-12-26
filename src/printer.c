#include "printer.h"

#define PRINTER_CMD_PRINTCHR       2


int is_printer_installed(dtpreg_t* printer_reg) {
    return printer_reg->status != DEVICE_ST_NOT_INSTALLED;
}

int printer_putchar(dtpreg_t* printer_reg, char c)
{
    unsigned int status;

    status = printer_reg->status;
    if (status != DEVICE_ST_READY)
        return -1;

    printer_reg->data0 = c;
    printer_reg->command = PRINTER_CMD_PRINTCHR;

    while ((status = printer_reg->status) == DEVICE_ST_BUSY)
        ;

    printer_reg->command = DEVICE_CMD_ACK;

    if (status != DEVICE_ST_READY) return -1;
    else return 0;
}

dtpreg_t *get_printer(unsigned int device_index) {
    return ((dtpreg_t *) DEV_REG_ADDR(IL_PRINTER, device_index));
}
