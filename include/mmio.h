#ifndef _mmio_h_
#define _mmio_h_

#include "types.h"

#define mm_writeb(addr, val) *(volatile uint8_t *)(addr) = val
#define mm_readb(addr) *(volatile uint8_t *)(addr)
#define mm_writew(addr, val) *(volatile uint32_t *)(addr) = val
#define mm_readw(addr) *(volatile uint32_t *)(addr)

#endif
