#include "../include/riscv.h"
#include "../include/spinlock.h"
#include "../include/trap_handle.h"
#include "../include/printk.h"
#include "../include/types.h"
#include "../include/plic.h"
#include "../include/uart.h"
#include "../include/disk.h"
#include "../include/proc.h"


void ktrap();
struct spinlock tick_lock;
unsigned ticks;

void clock_isr(){
  acquire_spinlock(&tick_lock);
  ticks++;
//   wakeup(&ticks);
  release_spinlock(&tick_lock);
}


void kernel_trap(){
    uint64_t cause = read_scause();
    uint64_t exp_code = GET_EXP_CODE(cause);
    if (IS_INTERRUPT(cause)){
        if (exp_code == CAUSE_EXTI){
            int irq = trap_claim();
            if(irq == UART0_IRQ){
                uart_isr();
            } else if(irq == VIRTIO0_IRQ){
                disk_isr();
            } else if(irq){
                printk("unexpected interrupt irq=%d\n", irq);
            }
            trap_complete(irq);
        }else if (exp_code == CAUSE_SOFT){
            if(get_coreid() == 0)
                clock_isr();
            write_sip(read_sip() & ~2);
            if(get_myproc() != 0 && get_myproc()->state == RUNNING);
                // yield();
        } else{
            // error
        }
    }


  // give up the CPU if this is a timer interrupt.

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
//   write_sepc(sepc);
//   write_sstatus(sstatus);
}

void user_trap(){
    // printk("USER TRAP\n");
    /* Now in kernel, use kernel trap instead */
    // write_stvec((uint64_t)ktrap);

    // /** 
    //  * The scause register is an XLEN-bit read-write register.
    //  * When a trap is taken into S-mode, 
    //  * scause is written with a code indicating the event that caused 
    //  * the trap. Otherwise, scause is never written by the 
    //  * implementation, though it may be explicitly written by software.
    //  * The Interrupt bit in the scause register is set if the contains a 
    //  * code identifying the last exception.
    //  */
    //  uint64_t trap_cause = read_scause();
    //  uint64_t exp_code = GET_EXP_CODE(trap_cause);
    //  if (IS_INTERRUPT(trap_cause)){
    //     int irq = 0;
    //     if (exp_code == CAUSE_EXTI){
    //         irq = trap_claim();
    //         printk("claim: %d\n", irq);
    //         if (irq == UART0_IRQ){
    //             printk("RRTTT\n");
    //             // for (;;);

    //             uart_isr();

    //         } else if (irq == VIRTIO0_IRQ) {
    //             printk("R2RTTT\n");
    //             for (;;);

    //             disk_isr();
    //         } else {
    //             printk("IRQ: %d\n", irq);
    //             for (;;);

    //         }
    //     } else if (exp_code == CAUSE_SOFT) {
    //             printk("LLPOP\n");

            
    //     }
    //     trap_complete(irq);
    //  } else {
    //     printk("RRTTT3\n");
    //     for (;;);

    //     if (exp_code == CAUSE_ENV_CALL){
    //         printk("Sys Call!");
    //     }
    //  }

}

void trap_init(){
    printk("+------------------------------------------+\n");
    printk("|               trap_init                  |\n");
    printk("+------------------------------------------+\n");
    spinlock_init(&tick_lock);
    write_stvec((uint64_t)ktrap);
}