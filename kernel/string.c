#include "../include/types.h"
#include "../include/string.h"

void* memset(void *ptr, int c, unsigned int len){
    int i;
    char *dst = (char *) ptr;
    for(i = 0; i < len; i++)
        dst[i] = c;
    return dst;
}