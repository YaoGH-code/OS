/*
 * main.c - Kernel entry point
 *
 * This file contains the main entry point for the kernel. It initializes
 * the UART, sets up the printk function, and performs the initial boot
 * sequence.
 */
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
#include "../include/vm.h"

int main(){
    if (!get_coreid()){
        uart_init();
        printk_init();
        printk("\n");
        printk("Kernel is booting...\n");
        pm_init();
        printk("%x\n",123);

        printk("%x\n",addr_round(0x12345678, 4096, 0));
        printk("%x\n",addr_round(0x12345678, 4096, 1));

    }
    




    return 0;
};