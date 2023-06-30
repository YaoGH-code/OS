#include "../include/uart.h"
#include "../include/console.h"
#include "../include/spinlock.h"
#include "../include/uart.h"
#include "../include/types.h"
#include "../include/printk.h"

#define LINESIZE 16
static char line[LINESIZE];
static int head = 0;
static int tail = 0;
static int edit = 0;
struct spinlock console_lock;

void console_putc(int c){
    if (c==0x100){
        uart_putc_sync('\b'); 
        uart_putc_sync(' '); 
        uart_putc_sync('\b');
    } else {
        uart_putc_sync((char) c);
    }
}

#define BACKSPACE '\b'
#define Ctrl(x)  ((x)-'@')  // Control-x

// called by uart isr
void console_isr(char c) {
    acquire_spinlock(&console_lock);

    switch (c)
    {
        case Ctrl('P'):
            printk("procdump\n");
            break;
        
        case Ctrl('U'):
            while(edit != head &&
                  line[(edit-1) % LINESIZE] != '\n'){
              edit--;
              console_putc(BACKSPACE);
            }
        
        case Ctrl('H'): // Backspace
        case '\x7f': // Delete key
            if(edit != head){
              edit--;
              console_putc(BACKSPACE);
            }
            break;
        default:
            if (c != 0 && edit - tail < LINESIZE) {
                c = (c == '\r') ? '\n' : c;

                // always echo back to the user
                console_putc(c);

                // store for consumption by processes who's waiting on console input
                line[edit++ % LINESIZE] = c;

                // wake up
                if (c == '\n')
                    edit = 0;
            }
            else {
                console_putc('\n');
                console_putc(c);
                edit = 0;
            }
            break;
    }

    release_spinlock(&console_lock);
    
}

void console_init(void)
{
  spinlock_init(&console_lock);
  uart_init();

//   // connect read and write system calls
//   // to consoleread and consolewrite.
//   devsw[CONSOLE].read = consoleread;
//   devsw[CONSOLE].write = consolewrite;
}