#include "devices/devices.h"
#include "core/system/architecture.h"

// Semaphores for device waiting. One for each distinct device
int s_io[N_EXT_IL][N_DEV_PER_IL];


int* get_dev_sem(unsigned int line, unsigned int instance) {
    return &s_io[line][instance];
}



