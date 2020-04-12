#ifndef BIKAYA_PHASE0_ARCHITECTURE_H
#define BIKAYA_PHASE0_ARCHITECTURE_H


#ifdef TARGET_UMPS

// Architecture-specific headers
#include "libumps.h"
#include "arch.h"
#include "types.h"
#include "cp0.h"

// MPS only defines NULL in stddef.h
#define NULL            ((void*)0)

// Addresses for new and old areas
#define STATE_T_SIZE        140
#define INT_OLDAREA         (RAM_BASE)
#define INT_NEWAREA         (RAM_BASE + STATE_T_SIZE)
#define TLB_OLDAREA         (RAM_BASE + (2 * STATE_T_SIZE))
#define TLB_NEWAREA         (RAM_BASE + (3 * STATE_T_SIZE))
#define PGMTRAP_OLDAREA     (RAM_BASE + (4 * STATE_T_SIZE))
#define PGMTRAP_NEWAREA     (RAM_BASE + (5 * STATE_T_SIZE))
#define SYSBK_OLDAREA       (RAM_BASE + (6 * STATE_T_SIZE))
#define SYSBK_NEWAREA       (RAM_BASE + (7 * STATE_T_SIZE))


//the following three lines are copied from p1.5test_bikaya_v0.c
#define RAMBASE    *((unsigned int *)BUS_REG_RAM_BASE)      //BUS_REG_RAM_BASE and BUS_REG_RAM_SIZE are defined in arch.h
#define RAMSIZE    *((unsigned int *)BUS_REG_RAM_SIZE)      //as 0x10000000 and 0x10000004 respectively
#define RAM_TOP     (RAMBASE + RAMSIZE)

#define FRAMESIZE 1024
#define FRAME_SIZE 4096

#endif
#ifdef TARGET_UARM
// Architecture-specific headers
    #include "libuarm.h"
    #include "arch.h"
    #include "uARMtypes.h"

#endif


#endif //BIKAYA_PHASE0_ARCHITECTURE_H
