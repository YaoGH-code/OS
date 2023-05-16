#include "../include/kmalloc.h"
#include "../include/param.h"
#include "../include/proc.h"
#include "../include/riscv.h"
#include "../include/spinlock.h"
#include "../include/string.h"
#include "../include/timer.h"
#include "../include/types.h"
#include "../include/uart.h"


int main(){
    uart_init();

    putc_sync('a');
    putc_sync('b');
    putc_sync('c');




    return 0;
};