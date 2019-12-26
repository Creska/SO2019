#ifndef BIKAYA_PHASE0_TERMINAL_H
#define BIKAYA_PHASE0_TERMINAL_H

#include "system.h"

termreg_t* get_terminal(unsigned int device_index);

/* Writes a string to the given terminal */
void term_putstr(termreg_t* term_reg, const char *str);

/* Writes a char to the given terminal
 * If an error status is found or generated during the read the function returns -1, else 0 */
int term_putchar(termreg_t* term_reg, char c);

/* Reads a char from the given terminal
 * If an error status is found or generated during the read the function returns '/a',
 * else the read char is returned*/
char term_getchar(termreg_t* term_reg);


#endif
