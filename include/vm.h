#ifndef _vm_h_
#define _vm_h_
#include "../include/types.h"

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

void kernel_vm_init();

#endif