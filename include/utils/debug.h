#ifndef BIKAYA_PHASE0_DEBUG_H
#define BIKAYA_PHASE0_DEBUG_H

#include "devices/terminal.h"

#define DEBUG_MESSAGE_PREFIX ">    "


void print_log_prefix();
void print_log_suffix();

// Logs a message to terminal0
#define LOG(message)    \
print_log_prefix();     \
addokbuf(message);      \
print_log_suffix()


#ifdef DEBUG

// Buffer for val to str conversions used in debug log macros
char _debug_str_buf[256];

// Logs a message to terminal0 if on debug mode
#define DEBUG_LOG(message)  \
print_log_prefix();         \
addokbuf(message);          \
print_log_suffix()

// Logs an int value with a description to terminal0 if on debug mode
#define DEBUG_LOG_INT(description, val)     \
print_log_prefix();                         \
addokbuf(description);                      \
addokbuf(int_to_str(val, _debug_str_buf));  \
print_log_suffix()

// Logs an int value with a description to terminal0 if on debug mode
#define DEBUG_LOG_BININT(description, val)          \
print_log_prefix();                                 \
addokbuf(description);                              \
addokbuf(int_to_str_binary(val, _debug_str_buf));   \
print_log_suffix()

// Logs a pointer value with a description to terminal0 if on debug mode
#define DEBUG_LOG_PTR(description, val)     \
print_log_prefix();                         \
addokbuf(description);                      \
addokbuf(ptr_to_str(val, _debug_str_buf));  \
print_log_suffix()

#else

// Empty macro definitions for non-debug compilation, this way when DEBUG is not defined nothing is compiled
#define DEBUG_LOG(message)
#define DEBUG_LOG_INT(description, val)
#define DEBUG_LOG_PTR(description, val)
#endif


#endif //BIKAYA_PHASE0_DEBUG_H
