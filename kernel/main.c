#include "../include/kmalloc.h"
#include "../include/param.h"
#include "../include/proc.h"
#include "../include/riscv.h"
#include "../include/spinlock.h"
#include "../include/string.h"
#include "../include/timer.h"
#include "../include/types.h"

int main(){
    pminit();
    void *test = kmalloc();
    void *test1 = kmalloc();
    void *test2 = kmalloc();
    void *test3 = kmalloc();
    void *test4 = kmalloc();
    void *test5 = kmalloc();

    kfree(test);
    kfree(test1);
    kfree(test2);
    kfree(test3);



    return 0;
};