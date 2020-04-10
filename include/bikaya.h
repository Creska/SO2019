#ifndef BIKAYA_PHASE0_BIKAYA_H
#define BIKAYA_PHASE0_BIKAYA_H

// Initialization routine for the Bikaya OS
// This method needs to be called one time before any other operation. After that processes can be added to the scheduler and the system can be launched.
void bikaya_initialize();

void bikaya_quick_launch(void* method, unsigned int priority, unsigned int vm_on, unsigned int km_on, unsigned int int_timer_on, unsigned int other_int_on);

#endif //BIKAYA_PHASE0_BIKAYA_H
