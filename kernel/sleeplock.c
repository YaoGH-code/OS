#include "../include/sleeplock.h"
#include "../include/spinlock.h"
#include "../include/proc.h"

//FIXME

void initsleeplock(struct sleeplock *lk, char *name)
{
  spinlock_init(&lk->lk);
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

// void
// acquiresleep(struct sleeplock *lk)
// {
//   acquire_spinlock(&lk->lk);
//   while (lk->locked) {
//     // sleep(lk, &lk->lk);
//   }
//   lk->locked = 1;
//   lk->pid = get_myproc()->pid;
//   release_spinlock(&lk->lk);
// }

// void
// releasesleep(struct sleeplock *lk)
// {
//   acquire_spinlock(&lk->lk);
//   lk->locked = 0;
//   lk->pid = 0;
// //   wakeup(lk);
//   release_spinlock(&lk->lk);
// }

// int
// holdingsleep(struct sleeplock *lk)
// {
//   int r;
  
//   acquire_spinlock(&lk->lk);
//   r = lk->locked && (lk->pid == get_myproc()->pid);
//   release_spinlock(&lk->lk);
//   return r;
// }



