#include "../include/kerror.h"
#include "../include/printk.h"

void kerror(char *file_name, int line, const char *s) {
    printk("%s:%d kerror: %s\n", file_name, line, s);
    for (;;);
}