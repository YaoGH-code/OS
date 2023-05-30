#ifndef _vm_h_
#define _vm_h_
#include "../include/types.h"

typedef uint64_t* ptb_t;
typedef uint64_t pte_t;

/* length of one level of page table index */
#define PTIDX_LEN 9
/* virtual page offset */
#define PG_OFFSET 12
/* extract 9 bits page index */
#define PTIDX_MASK 0x1FF

#define PTE_V (1L << 0)
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4)

/* Allocate page table */
#define PG_ALLOC 1
/* Do not allocate page table */
#define PG_NOT_ALLOC 0

/* PTE valid bit */
#define PTE_VALID(pte) ((pte) & (PTE_V))
/* PTE read bit */
#define PTE_READ(pte) ((pte) & (PTE_R))
/* PTE write bit */
#define PTE_WRITE(pte) ((pte) & (PTE_W))
/* PTE execution bit */
#define PTE_EXE(pte) ((pte) & (PTE_X))
/* PTE user access */
#define PTE_USER(pte) ((pte) & (PTE_U))

/* sv39 allows to use 39 bits for virtual 
addresses but we only use 38 bits since 
this could avoid the sign extension of a 
39 bits address with bit 38 set. */
#define MAXVA (1L << 38)

/* Extract the page table index for a certain level */
#define GET_PT_IDX(va, level) ((va >> ((level * PTIDX_LEN) + PG_OFFSET)) & (PTIDX_MASK))
/* Round an address to a boundary */
#define ADDR_ROUND(address, target, dir) ((dir) ? ((address+target-1) & ~(target-1)) : (address & ~(target - 1)))
/* Get pa from PTE */
#define GET_PA(pte) (((pte) >> 10) << 12)
/* Get PTE from pa */
#define GET_PTE(pa) ((((uint64_t)pa) >> 12) << 10)

pte_t* search_pttree(ptb_t pagetable, uint64_t va, int alloc);
void kernel_vm_init();

#endif