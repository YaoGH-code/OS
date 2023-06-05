#include "../include/riscv.h"
#include "../include/spinlock.h"
#include "../include/trap_handle.h"
#include "../include/printk.h"

struct spinlock tick_lock;

void trap_init(){
    printk("+------------------------------------------+\n");
    printk("|               trap_init                  |\n");
    printk("+------------------------------------------+\n");
    spinlock_init(&tick_lock);
}