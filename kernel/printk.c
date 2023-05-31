/*
 * printk.c - Kernel printing functions
 *
 * This file contains the implementation of kernel printing functions
 * used for formatted output to the console or UART.
 */
#include "../include/types.h"
#include "../include/printk.h"
#include "../include/uart.h"
#include "../include/spinlock.h"
#include "../include/console.h"

struct spinlock pk_lock;

void printk_init(void){
    spinlock_init(&pk_lock);
}

void print_str(char *s) {
    int i=0; 
    while (s[i]){
        uart_putc_sync(s[i]);
        i++;
    }
}

void print_int(int n, int b){
    long pos_n;
    int i = 0;
    char buf[16];

    if (b == 16) {
        console_putc('0');
        console_putc('x');
    }
    
    if (n==0) {
        console_putc('0');
        return;
    }
    if (n<0) pos_n = -n;
    else pos_n = n;
    while (pos_n>0) {
        int rem = pos_n % b;
        if (rem > 9)
            buf[i] = 'a' + rem - 10;
        else
            buf[i] = '0' + rem;
        pos_n = pos_n/b;
        i++;
    }
    i--;
    if(n<0) console_putc('-');
    for (; i>=0; --i) console_putc(buf[i]);
}

void print_ptr(uint64_t x){
  int i=0;
  char buf[16];

  console_putc('0');
  console_putc('x');
  while (x>0) {
    int rem = x % 16;
    if (rem > 9)
        buf[i] = 'a' + rem - 10;
    else
        buf[i] = '0' + rem;
    x = x/16;
    i++;
  }
  i--;
  for (; i>=0; i--) console_putc(buf[i]);
}

void printk(const char *fmt, ...){
    acquire_spinlock(&pk_lock);
    int i=0;
    va_list p;
    va_start(p, fmt);
    while (fmt[i] != '\0') {
        if (fmt[i] == '%') {
            char type = fmt[++i];
            if (type == 's')
                print_str(va_arg(p, char*));
            else if (type == 'o')
                print_int(va_arg(p, int), 8);
            else if (type == 'd')
                print_int(va_arg(p, int), 10);
            else if (type == 'x')
                print_int(va_arg(p, int), 16);
            else if (type == 'p')
                print_ptr(va_arg(p, uint64_t));
        } else console_putc(fmt[i]);
        i++;
    }
    release_spinlock(&pk_lock);
}
