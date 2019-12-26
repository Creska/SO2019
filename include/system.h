#ifndef BIKAYA_PHASE0_SYSTEM_H
#define BIKAYA_PHASE0_SYSTEM_H

#ifdef TARGET_UMPS
    #include "libumps.h"
    #include "arch.h"
    #include "types.h"
#endif
#ifdef TARGET_UARM
    #include "libuarm.h"
    #include "arch.h"
    #include "uARMtypes.h"
#endif

#define DEVICE_ST_NOT_INSTALLED     0
#define DEVICE_ST_READY             1
#define DEVICE_ST_BUSY              3

#define DEVICE_CMD_ACK              1

#endif


