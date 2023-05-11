#include "../include/types.h"
#include "../include/string.h"

void* memset(void *ptr, int c, unsigned int n){
    int i;
    char *dst = (char *) ptr;

    for(i = 0; i < n; i++)
        dst[i] = c;
    return dst;
}