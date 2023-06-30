/*
 * uart.c - UART driver implementation
 *
 * This file contains the implementation of the UART driver
 */
#include <stdarg.h>
#include <stdint.h>
#include "../include/riscv.h"
#include "../include/mmio.h"
#include "../include/spinlock.h"
#include "../include/uart.h"
#include "../include/console.h"
#include "../include/printk.h"



struct spinlock uart_lock;

char buf[UART_BSIZE];
int head;
int tail;

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

char uart_get_char() {
    if (mm_readb((uint64_t)LSR) & 0x01)
        return mm_readb(RHR);
    else
        return -1;
}

void uart_putc(char c) {
    acquire_spinlock(&uart_lock);
    while (tail == head + UART_BSIZE);
    buf[tail % UART_BSIZE] = c;
    tail++;
    uart_flush();
    release_spinlock(&uart_lock);
}

/* Transmit a char through UART when transmitter hold register is empty by
   writing the char to THR register */
void uart_putc_sync(char c) {
    intr_push();
    while (!(mm_readb(LSR) & LSR_THR_IDLE));
    mm_writeb(THR, c);
    intr_pop();
}

void uart_isr(){
    int c = 0;
    while((c = uart_get_char()) != (char)-1)
        console_isr(c);
    uart_flush();
}

void uart_flush(){
    acquire_spinlock(&uart_lock);
    while (head != tail){
        if (!(mm_readb(LSR) & LSR_THR_IDLE))
            return;
        char c = buf[head];
        head++;
        mm_writeb(THR, c);
    }
    release_spinlock(&uart_lock);
}