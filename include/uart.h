#ifndef _uart_h_
#define _uart_h_

/* 
QEMU is using 16550: http://byterunner.com/16550.html 
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
#define LCR_EIGHT_BITS (3<<0)  // word length set to 8 bits (bit0=1 && bit1=1)
#define FCR_FIFO_ENABLE (1<<0) // Enable the transmit and receive FIFO. This bit should be enabled before setting the FIFO trigger levels
#define FCR_FIFO_CLEAR (3<<1)  // clear the content of the two FIFOs
#define IER_RX_ENABLE (1<<0)   // enable the receiver ready interrupt
#define IER_TX_ENABLE (1<<1)   // enable the transmitter empty interrupt
#define LSR_THR_IDLE (1<<5)  // transmitter hold register (or FIFO) is empty, CPU can load the next character

#define LSR_RDR_READY (1<<0)

#define UART_BSIZE 32

void uart_init(void);
char uart_get_char();
void uart_putc(char c);
void uart_putc_sync(char c);
void uart_isr(void);
void uart_flush();

#endif