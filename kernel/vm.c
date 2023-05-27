/*
 * vm.c
 * 
 */
#include "../include/vm.h"
#include "../include/kerror.h"
#include "../include/kmalloc.h"
#include "../include/riscv.h"
#include "../include/string.h"

/* 
 * The risc-v Sv39 scheme uses three levels of page-table
 * pages. A page-table page contains 512 64-bit PTEs.
 * Virtual Address Structure:
 * +-------------+---------------+---------------+---------------+---------------------+
 * |     25      |       9       |       9       |       9       |          12         |
 * +-------------+---------------+---------------+---------------+---------------------+
 * |    39-63    |     30-38     |     21-29     |     12-20     |         0-11        |
 * +-------------+---------------+---------------+---------------+---------------------+
 * |  Must be 0  | Level-2 Index | Level-1 Index | Level-0 Index | Byte Offset in Page |
 * +-------------+---------------+---------------+---------------+---------------------+
 */

ptb_t kernel_ptb;

pte_t* search_pttree(ptb_t pagetable, uint64_t va, int alloc){
    if (va > MAXVA) 
        kerror("[vm.c] search_pttree: Illegal virtual memory address \n");

}

void kernel_vm_init(){
    kernel_ptb = (ptb_t) kmalloc();
    memset((void*)kernel_ptb, 0, PPSIZE);

}







