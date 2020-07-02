#ifndef BIKAYA_SYSTEM_H
#define BIKAYA_SYSTEM_H

#include "core/system/architecture.h"


// Identify the exception type
enum exc_type {SYS, PRG, TLB, INT};

// AREA AGES: they identify the type of area: old/new
enum area_age {NEW, OLD};

// EXCEPTION CODES: the allow to determine the cause of an exception in an arch-independent way
enum exc_code {E_BP, E_SYS, E_OTHER};

// Memory address
typedef unsigned int memaddr;


// ARCHITECTURE-INDEPENDENT UTILITY METHODS ===========================================================================

// Bus and system-wide ------------------------------------------------------------------------------------------------

// A pointer to the TOD_LO register (the only one functional)
#define TOD *(unsigned int*)BUS_REG_TOD_LO

// A pointer to the interval timer. Setting its value causes an acknowledgement of an interrupt on it's line.
#define INTERVAL_TIMER  (*(unsigned int*)BUS_REG_TIMER)

// A pointer to a system new/old area
#define AREA(new_old, exc_type)    ((state_t*)(AREA_BASE + (7 - exc_type*2 - new_old)*STATE_T_SIZE))

// Initialize the given area with the appropriate flags (for example kernel mode on, interrupts off), setting the PC
// to the given address
void init_new_area(state_t* area, void (*handler)());

// Returns 1 if an interrupt is pending on the given line, 0 otherwise
unsigned int is_interrupt_pending(unsigned int line);

// Returns the number of clock ticks corresponding to a time period defined in microseconds
unsigned int clock_ticks_per_period(unsigned int microseconds);




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















