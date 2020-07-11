#ifndef BIKAYA_DEBUG_H
#define BIKAYA_DEBUG_H

#include "devices/terminal.h"

#define DEBUG_MESSAGE_PREFIX "> "

void start_log();
void end_log();

// If in DEBUG mode compile the following macros with functionality, else make them empty,
// they will be ignored by the compiler
#ifdef DEBUG

// Buffer for val to str conversions used in debug log macros
static char debug_str_buf[256];

// Logs a message to terminal0 if on debug mode
#define DEBUG_LOG(message)  \
start_log();                \
addokbuf(message);          \
end_log()

#define DEBUG_SPACING     \
start_log();              \
end_log()

// Logs an int value with a description to terminal0 if on debug mode
#define DEBUG_LOG_INT(description, val)     \
start_log();                                \
addokbuf(description);                      \
addokbuf(int_to_str(val, debug_str_buf));  \
end_log()

#define DEBUG_LOG_UINT(description, val)     \
start_log();                                \
addokbuf(description);                      \
addokbuf(uint_to_str(val, debug_str_buf)); \
end_log()

// Logs an int value with a description to terminal0 if on debug mode
#define DEBUG_LOG_BININT(description, val)          \
start_log();                                 \
addokbuf(description);                              \
addokbuf(int_to_str_binary(val, debug_str_buf));   \
end_log()

#else

// Empty macro definitions for non-debug compilation, this way when DEBUG is not defined nothing is compiled
#define DEBUG_LOG(message)
#define DEBUG_LOG_INT(description, val)
#define DEBUG_LOG_BININT(description, val)
#define DEBUG_LOG_UINT(description, val)
#define DEBUG_LOG_PTR(description, val)
#define DEBUG_SPACING

#endif

#endif //BIKAYA_DEBUG_H
