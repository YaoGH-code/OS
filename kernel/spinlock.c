/*
 * spinlock.c - Spinlock
 *
 */

#include "../include/riscv.h"
#include "../include/spinlock.h"
#include "../include/proc.h"
#include "../include/kerror.h"

/*
However, the interrupt may already be disabled before the first time of 
acquiring the lock. Also, we have to deal will nested acquiring lock 
operation which means we do not want interrupt to be enabled right
after each call of the release_spinlock function.
Therefore, we have to record the level of acquiring the spinlock or
disabling interrupt. 
In intr_push function, we first increment disable_cnt by one and record 
the sstatus when first time try to disable interrupt for later 
restore... (continue on intr_pop function)
*/
void intr_push(){
    /* Read current sstatus */
    uint64_t cur_sstatus = read_sstatus();
    /* If SIE, old_state = 1; if not SIE, old_state = 0 */
    int old_state = ((cur_sstatus & SSTATUS_SIE) != 0);
    /* Turn off interrupt */
    write_sstatus(read_sstatus() & ~SSTATUS_SIE);
    /* If in first level of interrupt, store current intrrupt state */
    if(!get_mycore()->disable_cnt) get_mycore()->prev_int_state = old_state;
    /* Disable count ++ */
    get_mycore()->disable_cnt++;
}

/* 
Each time we release a lock, we decrement one level of disabling 
interrupt. But, only restore sstatus when we reach 0 level of 
disabling interrupt because at this moment, there is no need to 
worry about interrupt and we have to restore the status before 
the first time of acquiring the lock.
*/
void intr_pop(){
    /* Get current core */
    struct core* core = get_mycore();
    /* Read current sstatus */
    uint64_t cur_sstatus = read_sstatus();
    /* If SIE, old_state = 1; if not SIE, old_state = 0 */
    int old_state = ((cur_sstatus & SSTATUS_SIE) != 0);
    if (old_state);
        // kerror();          
        
    if (core->disable_cnt < 1);
        // kerror();          

    /* Outter most level of disable, turn on interrupt if it is 
    on previously. */
    if (core->disable_cnt == 0 && core->prev_int_state == 1){
        write_sstatus(read_sstatus() | SSTATUS_SIE);
    }
    core->disable_cnt--;
}

/* Test if the current CPU is holding the lock */
int core_holding(struct spinlock *lock){
  return (lock->locked && lock->core == get_mycore());
}

/* Initialize the lock */
void spinlock_init(struct spinlock* lock){
    lock->locked = 0;
}

/* 
At the begining of the acquire function, we have to disable interrupt.
Imagine a case that when a interrupt handler acquired a lock, at the same
time, another intrrupt comes in and try to acquire the same lock. This is 
a situation that will cause deadlock. 
Therefore, we have to disable interrupt here and enbale interrupt when 
release the lock. 
By doing this, the current thread holding the lock will focus on current 
task and hopefully will not hold the lock very long since there is no 
timeslicing while lock is held and no interrupt will be processed.
*/
void acquire_spinlock(struct spinlock* lock){
    intr_push();
    if (core_holding(lock))
        kerror("there is already a core holding the spin lock.\n");          

    /* CAS */
    while(__sync_lock_test_and_set(&lock->locked, 1) != 0);
    /* 
    Memory fence:
    to ensure that the critical section's memory references 
    happen strictly after the lock is acquired.
    1. All reads before __sync_synchronize must complete before __sync_synchronize
    2. All writes after __sync_synchronize must complete after __sync_synchronize
    3. All reads before __sync_synchronize must commit to main memory
    4. All reads after  __sync_synchronize must read newest value from memory
    */
    __sync_synchronize();
    /* Update cpu */
    lock->core = get_mycore();
}

/* Release spin lock */
void release_spinlock(struct spinlock* lock){
    if (!core_holding(lock))
        kerror("there is not a core holding the spin lock.\n");                    

    lock->core = 0;
    __sync_synchronize(); // do not reorder into cs
    __sync_lock_release(&lock->locked);

    intr_pop();
}

