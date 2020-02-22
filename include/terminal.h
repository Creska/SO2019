#ifndef BIKAYA_PHASE0_TERMINAL_H
#define BIKAYA_PHASE0_TERMINAL_H


#define TERM_READ_ERROR_RETURN_CHAR    '\a'


typedef struct terminal terminal;

/* Retrieve a pointer to an opaque struct representing the terminal device */
terminal* get_terminal(unsigned int device_index);

/* Reads a char from the given terminal
 * If an error status is found or generated during the read the function returns '/a',
 * else the read char is returned*/
char term_getchar(terminal* term);



/* TERMINAL PRINT functionality (not strictly needed, used just for debug strings */

/* Writes a string to the given terminal */
void term_putstr(terminal* term_reg, const char *str);

/* Writes a char to the given terminal
 * If an error status is found or generated during the read the function returns -1, else 0 */
int term_putchar(terminal* term, char c);


#endif
