#ifndef _printx_h_
#define _printx_h_
#include <stdarg.h>

void printk_init(void);
void print_str(char *s);
void printk(const char *fmt, ...);

#endif