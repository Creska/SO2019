#ifndef BIKAYA_SYSTEM_H
#define BIKAYA_SYSTEM_H

#include "core/system/architecture.h"


// Identify the exception type
enum exc_type {SYS, PRG, TLB, INT};

// AREA AGES: they identify the type of area: old/new
enum area_age {NEW, OLD};

// EXCEPTION CODES: the allow to determine the cause of an exception in an arch-independent way
enum exc_code {E_BP, E_SYS, E_OTHER};

// A pointer to a system new/old area
#define GET_AREA(new_old, exc_type)    ((state_t*)(AREA_BASE + (7 - exc_type*2 - new_old)*STATE_T_SIZE))

typedef unsigned int memaddr;


// ARCHITECTURE-INDEPENDENT UTILITY METHODS ===========================================================================

// CPU state manipulation ---------------------------------------------------------------------------------------------

// Initialize all the state values to 0
void reset_state(state_t* s);

// Sets the kernel bit in the status register of the given CPU state
void set_kernel_mode(state_t* s, unsigned int on);

// Sets the Virtual Memory bit in the status register of the given CPU state
void set_virtual_mem(state_t* s, unsigned int on);

// Set interval timer interrupts
void set_interval_timer_interrupts(state_t* s, unsigned int on);

// Sets all interrupts other than the interval timer's
void set_other_interrupts(state_t* s, unsigned int on);

// Sets the stack pointer value for the given state
void set_sp(state_t* s, unsigned int sp_val);

// Sets the program counter value for the given state
void set_pc(state_t* s, void (*ptr)());

// Returns 1 if an interrupt is pending on the given line, 0 otherwise
unsigned int is_interrupt_pending(unsigned int line);

// Returns the device's instance
#define GET_DEV_INSTANCE(reg) ((reg - DEV_REG_START)/(DEV_REG_SIZE)) % N_DEV_PER_IL

// Returns the device line's number
#define GET_DEV_LINE(reg) ((reg - DEV_REG_START)/DEV_REG_SIZE)/N_DEV_PER_IL + GET_DEV_START

#define GET_DEV_REL_LINE(reg) ((reg - DEV_REG_START)/DEV_REG_SIZE)/N_DEV_PER_IL

// A pointer to the TOD_LO register (the only one functional)
#define TOD *(unsigned int*)BUS_REG_TOD_LO

// BUS Register -------------------------------------------------------------------------------------------------------

// Sets the value of the interval timer
void set_interval_timer(unsigned int val);

#define set_interval_timer_macro(val) \
*(unsigned int*)BUS_REG_TIMER = val;

#define get_interval_timer_macro()  \
*(unsigned int*)BUS_REG_TIMER



// System initialization routines -------------------------------------------------------------------------------------

void init_new_area(state_t* area, void (*handler)());

// Returns the number of clock ticks expected to happen in a time period defined in microseconds
//
// Remarks: the TIME_SCALE (number of ticks per microseconds) is set at boot/reset, so we don't need to worry about it changing during execution.
unsigned int clock_ticks_per_period(unsigned int microseconds);


// Returns an unsigned integer code corresponding to the cause of an exeption as defined in EXCODE_... macros
enum exc_code get_exccode(state_t* state);


#ifdef TARGET_UMPS

    #define SYSCALL_RET_REG(state) (state)->reg_v0
    #define SYSCALL_N(state)       (state)->reg_a0
    #define SYSCALL_ARG1(state)    (state)->reg_a1
    #define SYSCALL_ARG2(state)    (state)->reg_a2
    #define SYSCALL_ARG3(state)    (state)->reg_a3

#elif TARGET_UARM

    #define SYSCALL_RET_REG(state) (state)->a1
    #define SYSCALL_N(state)       (state)->a1
    #define SYSCALL_ARG1(state)    (state)->a2
    #define SYSCALL_ARG2(state)    (state)->a3
    #define SYSCALL_ARG3(state)    (state)->a4

#endif


#endif


