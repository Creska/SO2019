#ifndef BIKAYA_PHASE0_UTILS_H
#define BIKAYA_PHASE0_UTILS_H

#include "core/system/system.h"

// Sets some bits of a given unsigned int
//
// original:    is the original value
//
unsigned int set_bits(unsigned int original, unsigned int bit, unsigned int value);

// Our implementation of memcpy
void* memcpy(void* target_s, const void* source_s, unsigned int bytes);


#endif //BIKAYA_PHASE0_UTILS_H
