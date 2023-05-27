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


int main(){
    if (!get_coreid()){
        uart_init();
        printk_init();
        printk("\n");
        printk("Kernel is booting...\n");
        pm_init();
        int *i = (int*) kmalloc();
        for (int j=0; j<10; j++){
            printk("%d\n", i[j]);


        }

    }
    




    return 0;
};