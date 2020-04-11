#include "utils/debug.h"

void print_log_prefix() {
    addokbuf(DEBUG_MESSAGE_PREFIX);
}

void print_log_suffix() {
    addokbuf("\n");
}

