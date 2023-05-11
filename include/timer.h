#ifndef _timer_h_
#define _timer_h_

#define CLINT 0x2000000L
#define TIMER_INTERVAL 1000000
#define CLINT_MTIME 0x200BFF8L
#define MTIMECMP_BASE 0x2004000L

/* mtimecmp for different cores starting from 0x2004000 */
#define CLINT_MTIMECMP(hartid) (MTIMECMP_BASE + 8*(hartid))

void timer_init();

#endif
