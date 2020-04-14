#ifndef BIKAYA_PROCESS_INITIALIZATION_DATA_H
#define BIKAYA_PROCESS_INITIALIZATION_DATA_H

// Structure holding all the relevant data for the initialization of a process
struct process_initialization_data {
    // A pointer to the entry point of the process
    void* method;
    // The (original) priority of the process
    int priority;
    // Virtual memory 0->OFF, 1->ON
    unsigned int vm_on;
    // Kernel mode 0->OFF, 1->ON
    unsigned int km_on;
    // Interval timer interrupts 0->OFF, 1->ON
    unsigned int timer_int_on;
    // Interrupts other than the interval timer 0->OFF, 1->ON
    unsigned int other_ints_on;
};

typedef struct process_initialization_data proc_init_data;


#endif //BIKAYA_PROCESS_INITIALIZATION_DATA_H
