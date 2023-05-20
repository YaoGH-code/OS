#include <stdarg.h>
#include "../include/riscv.h"
#include "../include/mmio.h"
#include "../include/spinlock.h"

/* 
QEMU is using 16550, detailed register map is on 
http://byterunner.com/16550.html 
*/
#define REG_ADDR(offset) UART_BASE + offset
#define RHR UART_BASE   // Receive holding register (for input bytes)
#define THR UART_BASE   // Transmit holding register (for output bytes)
#define DLL UART_BASE   // Divisor latch low
#define DLH UART_BASE+1 // Divisor latch high
#define IER UART_BASE+1 // Interrupt enable register
#define FCR UART_BASE+2 // FIFO control register
#define ISR UART_BASE+2 // Interrupt status register
#define LCR UART_BASE+3 // Line control register
#define LSR UART_BASE+5 // Line status register

#define LCR_LATCH (1<<7)       // special mode to set baud rate
#define LCR_EIGHT_BITS (3<<0)
#define FCR_FIFO_ENABLE (1<<0)
#define FCR_FIFO_CLEAR (3<<1)  // clear the content of the two FIFOs
#define IER_RX_ENABLE (1<<0)
#define IER_TX_ENABLE (1<<1)
#define LSR_THR_IDLE (1 << 5)

struct spinlock lock;

void uart_init(void){
    /* Initialize the lock */
    spinlock_init(&lock);
    /* disable the receiver ready interrupt */
    mm_writeb(IER, 0);
    /* BAUD LATCH enabled */
    mm_writeb(LCR, LCR_LATCH);
    /* baud rate of 38.4K */
    mm_writeb(DLL, 3);
    mm_writeb(DLH, 0);
    /* Set word length to 8 bits */
    mm_writeb(LCR, LCR_EIGHT_BITS);
    /* FIFO enable and clear */
    mm_writeb(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);
    /* transmit holding register interrupt	
    receive holding register interrupt*/
    mm_writeb(IER, IER_TX_ENABLE | IER_RX_ENABLE);
}

void uart_putc_sync(char c) {
    intr_push();
    while (!(mm_readb(LSR) & LSR_THR_IDLE));
    mm_writeb(THR, c);
    intr_pop();
}