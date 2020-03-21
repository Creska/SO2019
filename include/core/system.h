#ifndef BIKAYA_PHASE0_SYSTEM_H
#define BIKAYA_PHASE0_SYSTEM_H

// Includes architecture specific headers
#ifdef TARGET_UMPS
    #include "libumps.h"
    #include "arch.h"
    #include "types.h"

    // MPS only defines NULL in stddef.h
    #define NULL ((void*)0)
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


// BUS Register -------------------------------------------------------------------------------------------------------

// Sets the value of the interval timer
void set_interval_timer(unsigned int val);


// New/Old areas ------------------------------------------------------------------------------------------------------
// Get pointers to the new areas

state_t* get_new_area_sys_break();
state_t* get_new_area_program_trap();
state_t* get_new_area_TLB();
state_t* get_new_area_interrupt();

#endif


