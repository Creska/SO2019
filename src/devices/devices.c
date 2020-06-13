#include "devices/devices.h"
#include "core/system/architecture.h"
#include "utils/debug.h"


enum ext_dev_type get_ext_dev_type(unsigned int line, unsigned int subdev) {
#ifdef DEBUG
    if (line >= N_IL || line < (N_IL - N_EXT_IL)) {
        DEBUG_LOG_UINT("The given line doesn't correspond to an external device: ", line);
        HALT();
    }
#endif
    enum ext_dev_type t = line - (N_IL-N_EXT_IL);
    if (line == IL_TERMINAL) t += subdev;
    return t;
}

unsigned int get_ext_dev_line(enum ext_dev_type dev_type) {
    unsigned int l = dev_type;
    if (l==TERM_RX) l = TERM_TX;
    return l + 3;
}








