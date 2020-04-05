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


/* This function places the specified character string in okbuf and
 *	causes the string to be written out to terminal0 */
void addokbuf(char *strp);

/* This function places the specified character string in errbuf and
 *	causes the string to be written out to terminal0.  After this is done
 *	the system shuts down with a panic message */
void adderrbuf(char *strp);


// Values to string conversions ---------------------------------------------------------------------------------------

// Prints a string representation of an int in a buffer
// returns a pointer to the given buffer
char* int_to_str(int i, char *b);

// Prints a binary string representation of an int in a buffer
// returns a pointer to the given buffer
char* int_to_str_binary(int i, char *b);

// Prints a string representation of a pointer in a buffer
// returns a pointer to the given buffer
char* ptr_to_str(void* p, char *b);

// Prints a string representation of a number on a string buffer
//
// i                the target number
// digit            a string containing all the
// base             the target base for the number
// b                a char pointer to the address where this method should start writing the string on the buffer
//
// returns          a pointer to the char following the last one written in the buffer by this method
//
// Remarks:
//      - this method doesn't terminate the string
char* num_to_str_buf(int i, const char digit[], int base, char* b);


// LOG FUNCTIONALITY --------------------------------------------------------------------------------------------------

// Logs a message to terminal0
#define LOG(message)    \
addokbuf(message);      \
addokbuf("\n")


#ifdef DEBUG

// Buffer for val to str conversions used in debug log macros
char _debug_str_buf[256];

// Logs a message to terminal0 if on debug mode
#define DEBUG_LOG(message)  \
addokbuf(message);          \
addokbuf("\n")

// Logs an int value with a description to terminal0 if on debug mode
#define DEBUG_LOG_INT(description, val)     \
addokbuf(description);                      \
addokbuf(int_to_str(val, _debug_str_buf));  \
addokbuf("\n")


// Logs an int value with a description to terminal0 if on debug mode
#define DEBUG_LOG_BININT(description, val)     \
addokbuf(description);                      \
addokbuf(int_to_str_binary(val, _debug_str_buf));  \
addokbuf("\n")


// Logs a pointer value with a description to terminal0 if on debug mode
#define DEBUG_LOG_PTR(description, val)     \
addokbuf(description);                      \
addokbuf(ptr_to_str(val, _debug_str_buf));  \
addokbuf("\n")

#else
#define DEBUG_LOG(message)                              // Empty macro definitions for non-debug compilation
#define DEBUG_LOG_INT(description, val)
#define DEBUG_LOG_PTR(description, val)
#endif


#endif
