#ifndef _plic_h_
#define _plic_h_

#define PLIC_PRIORITY_BASE 0xc000000L
#define PLIC_IRQ_EN_BASE 0xC002080 
#define PLIC_THRES_BASE 0xC201000
#define UART0_IRQ 10
#define VIRTIO0_IRQ 1

void plic_init();
void plic_init_hart();

#endif