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
#include "../include/trap_handle.h"
#include "../include/plic.h"
#include "../include/disk.h"
#include "../include/mmio.h"
#include "../include/bio.h"
#include "../include/fs.h"
#include "../include/console.h"

volatile static int started = 0;
extern ptb_t kernel_ptb;
extern char etext[];

void usertrap();

int main(){
    if (!get_coreid()){
        console_init();
        printk_init();
        printk("\n");
        printk("Kernel is booting...\n");
        pm_init();
        kernel_vm_init();
        proc_init();
        trap_init();
        plic_init();
        write_sstatus(read_sstatus()|1<<1);


        // binit();
        // iinit();
        // disk_init();
        // printk("Kernel is booting...\n");
        // __sync_synchronize();
        // started = 1;
        // printk("Ending...\n");
    } 
    // else {
    //     while(started == 0);
    //     __sync_synchronize();
    //     printk("hart %d starting\n", get_coreid());
    // }
    for(;;);
    return 0;
};