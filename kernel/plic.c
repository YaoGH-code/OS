#include "../include/plic.h"
#include "../include/mmio.h"
#include "../include/riscv.h"
#include "../include/printk.h"

/**
 * Each PLIC interrupt source can be assigned a priority 
 * by writing to its 32-bit memory-mapped priority 
 * register. The FU540-C000 supports 7 levels of 
 * priority. A priority value of 0 is reserved to mean 
 * "never interrupt" and effectively disables the 
 * interrupt. Priority 1 is the lowest active priority, 
 * and priority 7 is the highest. Ties between global 
 * interrupts of the same priority are broken by the 
 * Interrupt ID; interrupts with the lowest ID have the 
 * highest effective priority.
 */
void plic_init(){
    printk("+------------------------------------------+\n");
    printk("|               plic_init                  |\n");
    printk("+------------------------------------------+\n");
    mm_writew(PLIC_PRIORITY_BASE + 4 * UART0_IRQ, 1);
    mm_writew(PLIC_PRIORITY_BASE + 4 * VIRTIO0_IRQ, 1);
    plic_init_hart();
}

/**
 * Each global interrupt can be enabled by setting the 
 * corresponding bit in the enables registers. The 
 * enables registers are accessed as a contiguous 
 * array of 2 × 32-bit words, packed the same way as 
 * the pending bits. Bit 0 of enable word 0 represents 
 * the non-existent interrupt ID 0 and is hardwired to 
 * 0.
 *
 * The FU540-C000 supports setting of an interrupt 
 * priority threshold via the threshold register. The 
 * threshold is a WARL field, where the FU540-C000 
 * supports a maximum threshold of 7. The FU540-C000 
 * masks all PLIC interrupts of a priority less than 
 * or equal to threshold. For example, a threshold 
 * value of zero permits all interrupts with non-zero 
 * priority, whereas a value of 7 masks all interrupts.
 */
void plic_init_hart(){
    uint64_t hartid = read_tp();
    mm_writew(PLIC_IRQ_EN_BASE + hartid * 0x100, 1 << VIRTIO0_IRQ | 1 << UART0_IRQ);
    mm_writew(PLIC_THRES_BASE + hartid * 0x2000, 0);
}
