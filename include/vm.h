#ifndef _vm_h_
#define _vm_h_
#include "../include/types.h"

/* length of one level of page table index */
#define PTIDX_LEN 9
/* virtual page offset */
#define PG_OFFSET 12
/* extract 9 bits page index */
#define PTIDX_MASK 0x1FF

/* PTE valid bit */
#define PTE_V (1L << 0)
/* PTE read bit */
#define PTE_R (1L << 1)
/* PTE write bit */
#define PTE_W (1L << 2)
/* PTE execution bit */
#define PTE_X (1L << 3)
/* PTE user access */
#define PTE_U (1L << 4)
/* sv39 allows to use 39 bits for virtual 
addresses but we only use 38 bits since 
this could avoid the sign extension of a 
39 bits address with bit 38 set. */
#define MAXVA (1L << 38)


typedef uint64_t* ptb_t;
typedef uint64_t pte_t;

uint64_t get_pt_idx(uint64_t va, int level);
uint64_t addr_round(uint64_t address, uint64_t target, bool dir);
pte_t* search_pttree(ptb_t pagetable, uint64_t va, int alloc);
void kernel_vm_init();

#endif