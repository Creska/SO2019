#ifndef BIKAYA_PHASE0_SCHEDULER_H
#define BIKAYA_PHASE0_SCHEDULER_H


#define SCHEDULER_TIME_SLICE 300000


// Initialization routine for the scheduler
void init_scheduler();

// The method called right after an interval timer interrupt
void time_slice_expired_callback();


#endif //BIKAYA_PHASE0_SCHEDULER_H
