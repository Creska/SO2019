#include <utils/utils.h>
#include <core/system.h>


unsigned int set_bits(unsigned int original, unsigned int mask, unsigned int value){
    return (original & ~mask) | value;
}

void* mem_cpy(void *src, void *dst, unsigned int bytes) {
    char *c_src = (char *)src;
    char *c_dst =(char*)dst;
    if((c_src != NULL) && (c_dst != NULL)){
        while(bytes){
            *(c_dst++) = *(c_src++);         // Copy byte by byte
            --bytes;
        }
    } else {
        PANIC();
    }
    return dst;
}
