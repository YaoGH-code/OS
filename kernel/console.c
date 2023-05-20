#include "../include/uart.h"


void console_putc(int c){
    if (c==0x100){
        uart_putc_sync('\b'); 
        uart_putc_sync(' '); 
        uart_putc_sync('\b');
    } else {
        uart_putc_sync((char) c);
    }
}