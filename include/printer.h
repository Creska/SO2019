#ifndef BIKAYA_PHASE0_PRINTER_H
#define BIKAYA_PHASE0_PRINTER_H

#include "system.h"

dtpreg_t* get_printer(unsigned int device_index);

/* Writes a char to the given printer.
 * If an error status is found or generated during the read the function returns -1, else 0. */
int printer_putchar(dtpreg_t* printer_reg, char c);

int is_printer_installed(dtpreg_t* printer_reg);

#endif