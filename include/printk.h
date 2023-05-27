#ifndef _printk_h_
#define _printk_h_
#include <stdarg.h>

void printk_init(void);
void print_str(char *s);
void printk(const char *fmt, ...);

#endif