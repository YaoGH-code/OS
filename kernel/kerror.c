#include "../include/kerror.h"
#include "../include/printk.h"

void kerror(const char *s) {
    printk("kerror: %s\n", s);
    for (;;);
}