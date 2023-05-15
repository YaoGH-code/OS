/*
 * kalloc.c - Physical memory allocator
 *
 */
#include "../include/kmalloc.h"
#include "../include/riscv.h"
#include "../include/types.h"
#include "../include/string.h"
#include "../include/spinlock.h"

/*
    Memory layout:
        +------------------+ _end
        |       free       |
        +------------------+ _bss_end/_free_start
        |       bss        |
        +------------------+
        |       data       |
        +------------------+
        |      rodata      |
        +------------------+
        |       text       |
        +------------------+ 0x80000000
*/

/* Free memory starting point (in link.ld) */
extern char free_start[];
/* Free memory ending point (in link.ld) */
extern char end[];

/* Physical memory block */
struct block{
    struct block* next;
};

/* Physical memory */
struct {
  struct spinlock lock;
  struct block *freelist;
} memory;

/* Initialize memory struct and spinlock */
void pminit(){
    spinlock_init(&memory.lock);
    memory.freelist = 0;
    for (char* i = free_start; i + PPSIZE < end; i+=PPSIZE){
        kfree((void*) i);
    }
}

/* 
Allocate a 4096 bytes physical page
return valid PA if memory available or
return 0 if no available memory
*/
void* kmalloc(){
    struct block *b = 0;

    acquire_spinlock(&memory.lock);
    if (memory.freelist){
        b = memory.freelist;
        memory.freelist = memory.freelist->next;  
    }
    release_spinlock(&memory.lock);

    if (b)
        memset((char*)b, 5, PPSIZE);
    return (void*)b;
}

/* Free a 4096 bytes physical page pointed by pa */
void kfree(void *pa){
    if (((uint64_t)pa % PPSIZE) != 0 || (char *)pa < free_start || (char *)pa > end){
        //kerror();
    }

    memset(pa, 1, PPSIZE);
    struct block *b = (struct block *) pa;

    acquire_spinlock(&memory.lock);
    b->next = memory.freelist;
    memory.freelist = b;
    release_spinlock(&memory.lock);
}


