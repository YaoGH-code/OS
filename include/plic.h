#ifndef _plic_h_
#define _plic_h_

#define PLIC_PRIORITY_BASE 0xC000000
#define PLIC_SEN_BASE      0xC002080 
#define PLIC_STHRES_BASE   0xC201000
#define PLIC_SCLAIM        0xC201004

#define UART0_IRQ 10
#define VIRTIO0_IRQ 1

void plic_init();
void plic_init_hart();
int trap_claim();
void trap_complete(int irq);

#endif