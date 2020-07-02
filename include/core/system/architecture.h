#ifndef BIKAYA_ARCHITECTURE_H
#define BIKAYA_ARCHITECTURE_H


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

// Starting point for system's old/new areas
#define AREA_BASE           RAM_BASE

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

// Starting point for system's old/new areas
#define AREA_BASE EXCV_BASE

#endif

// Mask for the device status bits
#define DEV_STATUS_MASK 0x000000ff

// The first line corresponding to an external device
#define GET_DEV_START (N_INTERRUPT_LINES - N_EXT_IL)

#endif
