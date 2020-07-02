#include "devices/devices.h"


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

void send_command(enum ext_dev_type ext_dev, unsigned int command, devreg_t* dev_reg) {
    if  (ext_dev != TERM_TX){
        dev_reg->dtp.command = command;
    } else {
        dev_reg->term.transm_command = command;
    }
}

unsigned int get_status(enum ext_dev_type dev_type, devreg_t* dev_reg) {
    if (dev_type!=TERM_TX) {
        return dev_reg->dtp.status;
    } else {
        return dev_reg->term.transm_status;
    }
}








