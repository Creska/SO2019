#ifndef BIKAYA_PHASE0_UTILS_H
#define BIKAYA_PHASE0_UTILS_H

#include "core/system.h"

unsigned int set_bits(unsigned int original, unsigned int bit, unsigned int value);

// Our implementation of memcpy
void* mem_cpy(void* source_s, void* target_s, unsigned int bytes);


#endif //BIKAYA_PHASE0_UTILS_H
