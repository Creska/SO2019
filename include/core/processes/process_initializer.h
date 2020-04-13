#ifndef BIKAYA_PHASE0_PROCESS_INITIALIZER_H
#define BIKAYA_PHASE0_PROCESS_INITIALIZER_H

struct process_initializer {
    void* method;
    int priority;
    unsigned int vm_on, km_on, timer_int_on, other_ints_on;
};

typedef struct process_initializer proc_init_data;


#endif //BIKAYA_PHASE0_PROCESS_INITIALIZER_H
