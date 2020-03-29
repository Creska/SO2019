#ifndef BIKAYA_PHASE0_SYSTEM_H
#define BIKAYA_PHASE0_SYSTEM_H

// Includes architecture specific headers
#ifdef TARGET_UMPS
    #include "libumps.h"
    #include "arch.h"
    #include "types.h"
    #include "cp0.h"

    // MPS only defines NULL in stddef.h
    #define NULL            ((void*)0)

    // Addresses for new and old areas
    #define AREA_SIZE           140
    #define INT_OLDAREA         RAM_BASE
    #define INT_NEWAREA         (RAM_BASE + AREA_SIZE)
    #define TLB_OLDAREA         (RAM_BASE + (2 * AREA_SIZE))
    #define TLB_NEWAREA         (RAM_BASE + (3 * AREA_SIZE))
    #define PGMTRAP_OLDAREA     (RAM_BASE + (4 * AREA_SIZE))
    #define PGMTRAP_NEWAREA     (RAM_BASE + (5 * AREA_SIZE))
    #define SYSBK_OLDAREA       (RAM_BASE + (6 * AREA_SIZE))
    #define SYSBK_NEWAREA       (RAM_BASE + (7 * AREA_SIZE))


    #define SP_INDEX  28
    #define T_INDEX 26 //t9 register index in state's gpr vector
    //the following three lines are copied from p1.5test_bikaya_v0.c
    #define RAMBASE    *((unsigned int *)BUS_REG_RAM_BASE)      //BUS_REG_RAM_BASE and BUS_REG_RAM_SIZE are defined in arch.h
    #define RAMSIZE    *((unsigned int *)BUS_REG_RAM_SIZE)      //as 0x10000000 and 0x10000004 respectively
    #define RAM_TOP     (RAMBASE + RAMSIZE)

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


// ARCHITECTURE-INDEPENDENT UTILITY METHODS ===========================================================================


// CPU state manipulation ---------------------------------------------------------------------------------------------

// Initialize all the state values to 0
void reset_state(state_t* s);

// Sets the kernel bit in the status register of the given CPU state
void set_kernel_mode(state_t* s, unsigned int on);

// Sets the Virtual Memory bit in the status register of the given CPU state
void set_virtual_mem(state_t* s, unsigned int on);

// Sets the Virtual Memory bits in the status register of the given CPU state,
// mask is an 8-bit mask that enables/disables external interrupts
void set_interrupt_mask(state_t* s, unsigned int mask);

// Sets the stack pointer value for the given state
void set_sp(state_t* s, unsigned int sp_val);


void set_pc(state_t* s, void (*ptr)());


// BUS Register -------------------------------------------------------------------------------------------------------

// Sets the value of the interval timer
void set_interval_timer(unsigned int val);


// New/Old areas ------------------------------------------------------------------------------------------------------

// Gets a pointer to the system/break new area
state_t* get_new_area_sys_break();

// Gets a pointer to the program trap new area
state_t* get_new_area_program_trap();

// Gets a pointer to the TLB new area
state_t* get_new_area_TLB();

// Gets a pointer to the system/break new area
state_t* get_new_area_int();


// System initialization routines -------------------------------------------------------------------------------------

void init_area(state_t* area, unsigned int int_mask, void (*handler)());

#endif


