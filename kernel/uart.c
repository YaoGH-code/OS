/*
 * uart.c - UART driver implementation
 *
 * This file contains the implementation of the UART driver
 */
#include <stdarg.h>
#include "../include/riscv.h"
#include "../include/mmio.h"
#include "../include/spinlock.h"
#include "../include/uart.h"

struct spinlock uart_lock;

/* Initialization of the UART */
void uart_init(void){
    /* Initialize the lock */
    spinlock_init(&uart_lock);
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

/* Transmit a char through UART when transmitter hold register is empty by
   writing the char to THR register */
void uart_putc_sync(char c) {
    intr_push();
    while (!(mm_readb(LSR) & LSR_THR_IDLE));
    mm_writeb(THR, c);
    intr_pop();
}