#include "devices/devices.h"
#include "core/system/architecture.h"


unsigned int get_actual_line(unsigned int line, int subdev) {
    unsigned int actual_line = line - 3;
    if (line==IL_TERMINAL && subdev) {
        actual_line++;
    }
    return actual_line;
}

// Semaphores for device waiting. One for each distinct device
int s_io[N_EXT_IL+1][N_DEV_PER_IL];                               // TODO we need one more line for the second half of the terminal

0  ---
1  ---
2  ---
3  0
4  1
5  2
6  3
7  4
   5


int* get_dev_sem(unsigned int line, unsigned int instance) {
    return &s_io[line][instance];
}



