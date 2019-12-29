#include "printer.h"
#include "system.h"

struct printer {
    dtpreg_t *reg;
};


static printer printers[N_DEV_PER_IL];


int is_printer_installed(printer* printer) {
    return printer->reg->status != DEVICE_ST_NOT_INSTALLED;
}

int printer_putchar(printer* printer, char c)
{
    unsigned int status;
    dtpreg_t *printer_reg = printer->reg;

    status = printer_reg->status;
    if (status != DEVICE_ST_READY)
        return -1;

    printer_reg->data0 = c;
    printer_reg->command = DEVICE_CMD_FUNC_SPECIFIC;

    while ((status = printer_reg->status) == DEVICE_ST_BUSY)
        ;

    printer_reg->command = DEVICE_CMD_ACK;

    if (status != DEVICE_ST_READY) return -1;
    else return 0;
}

printer *get_printer(unsigned int device_index) {
    if (printers[device_index].reg==NULL) {
        printers[device_index].reg = ((dtpreg_t *) DEV_REG_ADDR(IL_PRINTER, device_index));
    }
    return &printers[device_index];
}
