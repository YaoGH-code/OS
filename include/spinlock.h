#ifndef _spinlock_h_
#define _spinlock_h_

/* spin lock */
struct spinlock{
  unsigned int locked;
  struct core *core;
};

void intr_push();
void intr_pop();
int cpu_holding(struct spinlock *lock);
void spinlock_init(struct spinlock* lock);
void acquire_spinlock(struct spinlock* lock);
void release_spinlock(struct spinlock* lock);

#endif
