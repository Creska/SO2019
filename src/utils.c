#include <utils/utils.h>

unsigned int set_bits(unsigned int original, unsigned int mask, unsigned int value){
    return (original & ~mask) | value;
}