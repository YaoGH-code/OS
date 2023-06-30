#include "../include/types.h"
#include "../include/string.h"
#include "../include/printk.h"

void* memset(void *ptr, int c, unsigned int len){
    char *dst = (char *) ptr;
    for(int i = 0; i < len; i++)
        dst[i] = c;
    return dst;
}