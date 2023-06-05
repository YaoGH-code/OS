/*
 * vm.c
 * 
 */

#include "../include/vm.h"
#include "../include/kmalloc.h"
#include "../include/kerror.h"
#include "../include/kmalloc.h"
#include "../include/riscv.h"
#include "../include/string.h"
#include "../include/printk.h"
#include "../include/types.h"

/* 
 * xv6 runs on Sv39 RISC-V, which means that only the bottom 39 bits of a 64-bit virtual 
 * address are used; the top 25 bits are not used.
 * The risc-v Sv39 scheme uses three levels of page-table pages. A page-table page 
 * contains 512 64-bit PTEs.
 * Each PTE contains a 44-bit physical page number (PPN) and some flags.
 * The paging hardware translates a virtual address by using the top 27 bits of the 39 
 * bits to index into the page table to find a PTE, and making a 56-bit physical address
 * whose top 44 bits come from the PPN in the PTE and whose bottom 12 bits are copied 
 * from the original virtual address. 
 *
 * Virtual Address Structure:
 * +-------------+---------------+---------------+---------------+---------------------+
 * |     25      |       9       |       9       |       9       |          12         |
 * +-------------+---------------+---------------+---------------+---------------------+
 * |    39-63    |     30-38     |     21-29     |     12-20     |         0-11        |
 * +-------------+---------------+---------------+---------------+---------------------+
 * |  Must be 0  | Level-2 Index | Level-1 Index | Level-0 Index | Byte Offset in Page |
 * +-------------+---------------+---------------+---------------+---------------------+
 */              

extern char etext[];
extern char end[];
extern char trap[];

ptb_t kernel_ptb;

/* Return the address of the PTE in page table pagetable that corresponds to virtual 
address va. Create any required page-table pages if needed. */
pte_t* search_pttree(ptb_t pagetable, uint64_t va, int alloc){
    if (va > MAXVA) 
        kerror(__FILE_NAME__,__LINE__,"Illegal virtual memory address");

    for (int level = 2; level > 0; level--){
        /* Get the index for the current level of page table */
        uint64_t idx = GET_PT_IDX(va, level);
        /* Get PTE from page table */
        pte_t* pte = &pagetable[idx];

        if (PTE_VALID(*pte)) {
            /* If this PTE is valid, then we get PA from this PTE and
               may going into the next level of page table using this 
               PA */
            pagetable = (ptb_t)GET_PA(*pte);
        } else {
            /* If it is not valid (not initialized), then allocate 
               and initialize the corresponding page table, return 
               0 */
            if(!alloc || (pagetable = (uint64_t*)kmalloc()) == 0) return 0;
            // printk("[vm.c] allocated page %p for level %d page table\n", pagetable, level-1);
            memset(pagetable, 0, PSIZE);
            *pte = GET_PTE(pagetable) | PTE_V;
        }
    }
    return &pagetable[GET_PT_IDX(va, 0)];
}


int map_pages(ptb_t pagetable, uint64_t va, uint64_t size, uint64_t pa, int perm, char* purp){
    if (size <= 0)
        kerror(__FILE_NAME__,__LINE__,"Incorrect size");
    uint64_t curr_va = ADDR_ROUND(va, PSIZE, 0);
    uint64_t end_va  = ADDR_ROUND((va+size), PSIZE, 0);
    printk("[vm.c] Mapping va[%p-%p] to pa[%p] for %s\n",curr_va, end_va, pa, purp);
    pte_t* pte;

    while (curr_va < end_va) {
        pte = search_pttree(pagetable, curr_va, PG_ALLOC);
        if (!pte) return 0;
        if (PTE_VALID(*pte)) 
            kerror(__FILE_NAME__,__LINE__,"PTE already valid");
        *pte = GET_PTE(pa) | perm | PTE_V;
        curr_va += PSIZE;
        pa += PSIZE;
    }
    return 1;
}


void kernel_vm_init(){
    kernel_ptb = (ptb_t) kmalloc();
    memset((void*)kernel_ptb, 0, PSIZE);

    printk("+------------------------------------------+\n");
    printk("|              kernel_vm_init              |\n");
    printk("+------------------------------------------+\n");

    // UART
    if (!map_pages(kernel_ptb, UART_BASE, PSIZE, UART_BASE, PTE_R | PTE_W, "UART"))
        kerror(__FILE_NAME__,__LINE__,"Error in mapping UART");
    
    // Text
    if (!map_pages(kernel_ptb, KBASE, (uint64_t)etext-KBASE, KBASE, PTE_R | PTE_X, "Kernel text"))
        kerror(__FILE_NAME__,__LINE__,"Error in mapping Kernel text");
    
    // Data
    if (!map_pages(kernel_ptb, (uint64_t)etext, (uint64_t)end-(uint64_t)etext, (uint64_t)etext, PTE_R | PTE_W, "Kernel data"))
        kerror(__FILE_NAME__,__LINE__,"Error in mapping Kernel data");
    
    // PLIC
    if (!map_pages(kernel_ptb, PLIC, 0x400000, PLIC, PTE_R | PTE_W, "PLIC"))
        kerror(__FILE_NAME__,__LINE__,"Error in mapping PLIC");
    
    // Virtio
    if (!map_pages(kernel_ptb, VIRTIO, PSIZE, VIRTIO, PTE_R | PTE_W, "Virtio"))
        kerror(__FILE_NAME__,__LINE__,"Error in mapping Virtio");
    
    /* Trap 
     * The trap is supposed to be at the second page from the start of the 
     * KER_BASE, so we mapped MAXVA-PSIZE to trap ehich is defined in the 
     * linker script 
     */
    if (!map_pages(kernel_ptb, MAXVA-PSIZE, PSIZE, (uint64_t)trap, PTE_R | PTE_X, "Trap"))
        kerror(__FILE_NAME__,__LINE__,"Error in mapping trap");

    asm ("sfence.vma zero, zero");
    write_satp((uint64_t)kernel_ptb >> 12 | (1L << 63));
    asm ("sfence.vma zero, zero");
}








