#ifndef BIKAYA_PHASE0_SYSTEM_H
#define BIKAYA_PHASE0_SYSTEM_H

#ifdef TARGET_UMPS
    #include "libumps.h"
    #include "arch.h"
    #include "types.h"
    #define NULL ((void*)0)                 /* MPS only defines NULL in stddef.h*/
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
#define DEVICE_CMD_FUNC_SPECIFIC    2

#endif


