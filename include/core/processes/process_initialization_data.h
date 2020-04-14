#ifndef BIKAYA_PROCESS_INITIALIZATION_DATA_H
#define BIKAYA_PROCESS_INITIALIZATION_DATA_H

// Structure holding all the relevant data for the initialization of a process
struct process_initialization_data {
    void* method;
    int priority;
    unsigned int vm_on, km_on, timer_int_on, other_ints_on;
};

typedef struct process_initialization_data proc_init_data;


#endif //BIKAYA_PROCESS_INITIALIZATION_DATA_H
