#include "utils/debug.h"
#include "core/exceptions/interrupts.h"
#include "core/system/system.h"


void start_log() {
    addokbuf("\t");
    addokbuf(DEBUG_MESSAGE_PREFIX);
    addokbuf("");
}

void end_log() {
    addokbuf("\n");
}


