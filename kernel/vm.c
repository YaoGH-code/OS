/*
 * vm.c
 * 
 */
#include "../include/vm.h"
#include "../include/kerror.h"
#include "../include/kmalloc.h"
#include "../include/riscv.h"
#include "../include/string.h"
#include <stdint.h>

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

/**
 * @brief  Extract the page table index for a certain level
 * @param  va virtual address
 * @param  page table level
 * @return page table index for one level
 */
uint64_t get_pt_idx(uint64_t va, int level){
    return (va >> (level * PTIDX_LEN + PG_OFFSET)) & PTIDX_MASK;
}

/**
 * @brief  Round an address to a boundary 
 * @param  dir rounding direction -> 1==up 0==down
 * @return Rounded address
 */
uint64_t addr_round(uint64_t address, uint64_t target, bool dir){
    return (dir) ? ((address+target-1) & ~(target-1)) : (address & ~(target - 1));
}

/* */
pte_t* search_pttree(ptb_t pagetable, uint64_t va, int alloc){
    if (va > MAXVA) 
        kerror("[vm.c] search_pttree: Illegal virtual memory address \n");
    

}

void kernel_vm_init(){
    kernel_ptb = (ptb_t) kmalloc();
    memset((void*)kernel_ptb, 0, PPSIZE);



}







