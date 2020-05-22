#ifndef BIKAYA_SYSTEM_H
#define BIKAYA_SYSTEM_H

#include "core/system/architecture.h"

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
#define GET_DEV_LINE_SYSTEM(reg) ((reg - DEV_REG_START)/DEV_REG_SIZE)/N_DEV_PER_IL + GET_DEV_START

#define GET_DEV_LINE(reg) ((reg - DEV_REG_START)/DEV_REG_SIZE)/N_DEV_PER_IL

#define TOD *(unsigned int*)BUS_REG_TOD_LO

// BUS Register -------------------------------------------------------------------------------------------------------

// Sets the value of the interval timer
void set_interval_timer(unsigned int val);

#define set_interval_timer_macro(val) \
*(unsigned int*)BUS_REG_TIMER = val;

#define get_interval_timer_macro()  \
*(unsigned int*)BUS_REG_TIMER



// New/Old areas ------------------------------------------------------------------------------------------------------

// Gets a pointer to the system/break new area
state_t* get_new_area_sys_break();

// Gets a pointer to the syscall/break old area
// (where the processor state is saved during a syscall/breakpoint exception)
state_t* get_old_area_sys_break();

// Gets a pointer to the program trap new area
state_t* get_new_area_program_trap();

// Gets a pointer to the program trap old area
// (where the processor state is saved during a program trap exception)
state_t* get_old_area_program_trap();

// Gets a pointer to the TLB new area
state_t* get_new_area_TLB();

// Gets a pointer to the program trap old area
// (where the processor state is saved during a TLB exception)
state_t* get_old_area_TLB();

// Gets a pointer to the system/break new area
state_t* get_new_area_int();

// Gets a pointer to the program trap old area
// (where the processor state is saved during a interrupt exception)
state_t* get_old_area_int();


// System initialization routines -------------------------------------------------------------------------------------

void init_new_area(state_t* area, void (*handler)());

// Returns the number of clock ticks expected to happen in a time period defined in microseconds
//
// Remarks: the TIME_SCALE (number of ticks per microseconds) is set at boot/reset, so we don't need to worry about it changing during execution.
unsigned int clock_ticks_per_period(unsigned int microseconds);


#define EXCODE_BP           1           // Codes used to determine the cause of an exception
#define EXCODE_SYS          2
#define EXCODE_OTHER        3           // this system will easily support more codes when needed

// Returns an unsigned integer code corresponding to the cause of an exeption as defined in EXCODE_... macros
unsigned int get_exccode(state_t* state);

void load_syscall_registers(state_t* s, unsigned int* n, unsigned int* a1, unsigned int* a2, unsigned int* a3);

unsigned int* sys_n(state_t* s);

void save_syscall_return_register(state_t *s, unsigned int return_val);

#endif


