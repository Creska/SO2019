#ifndef BIKAYA_PRINTER_H
#define BIKAYA_PRINTER_H


typedef struct printer printer;

/* Retrieve a pointer to an opaque struct representing the printer device */
printer* get_printer(unsigned int device_index);

/* Writes a char to the given printer.
 * If an error status is found or generated during the read the function returns -1, else 0. */
int printer_putchar(printer* printer_reg, char c);

/* Checks if the given printer is installed*/
int is_printer_installed(printer* printer);

#endif