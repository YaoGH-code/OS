#include "../include/kmalloc.h"
#include "../include/param.h"
#include "../include/proc.h"
#include "../include/riscv.h"
#include "../include/spinlock.h"
#include "../include/string.h"
#include "../include/timer.h"
#include "../include/types.h"
#include "../include/uart.h"
#include "../include/printk.h"


int main(){
    if (!get_coreid()){
        uart_init();
        printk_init();
        printk("xv6 kernel is booting\n");
        printk("\n");
    }
    




    return 0;
};