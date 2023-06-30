#include "../include/riscv.h"

.section trampsec
.globl trap
trap:
.align 4
.globl usertrap
usertrap:    
        /**
         * The sscratch register (Supervisor Scratch Register) is 
         * an XLEN-bit read/write register, dedicated for by the 
         * supervisor. Typically, sscratch is used to hold a 
         * pointer to the hart-local supervisor context while the 
         * hart is executing user code. At the beginning of a trap 
         * handler, sscratch is swapped with a user register to 
         * provide an initial working register. 
         * 
         * In this trap handler, we first swap the value in the 
         * csrrw and the a0, so we can use a0 for accessing the 
         * trapframe.
         */
        csrrw a0, sscratch, a0

        /** 
         * Each process has a separate trap frame but it's mapped 
         * to the same virtual address
         */
        li a0, 0x3fffffe000

        /**
          +----------------+
          |   TRAP FRAME   |
          +----------------+
          |   Kernel satp  | OFF: 0
          +----------------+
          |   Kernel sp    | OFF: 8
          +----------------+
          |   User trap    | OFF: 16
          +----------------+
          |      epc       | OFF: 24
          +----------------+
          | Kernel hartid  | OFF: 32
          +----------------+
          |  GP registers  | OFF: 40 - 280
          +----------------+
          */
        
        /* Save user registers in the trap frame
         which is defined in proc.h */
        sd ra,  40(a0)
        sd sp,  48(a0)
        sd gp,  56(a0)
        sd tp,  64(a0)
        sd t0,  72(a0)
        sd t1,  80(a0)
        sd t2,  88(a0)
        sd s0,  96(a0)
        sd s1,  104(a0)
        sd a1,  120(a0)
        sd a2,  128(a0)
        sd a3,  136(a0)
        sd a4,  144(a0)
        sd a5,  152(a0)
        sd a6,  160(a0)
        sd a7,  168(a0)
        sd s2,  176(a0)
        sd s3,  184(a0)
        sd s4,  192(a0)
        sd s5,  200(a0)
        sd s6,  208(a0)
        sd s7,  216(a0)
        sd s8,  224(a0)
        sd s9,  232(a0)
        sd s10, 240(a0)
        sd s11, 248(a0)
        sd t3,  256(a0)
        sd t4,  264(a0)
        sd t5,  272(a0)
        sd t6,  280(a0)

	     /* Save a0 from the user space to t0 since t0
           has been save to the trap frame */
        csrr t0, sscratch

        /* Save t0 (a0) to the trap frame to finish saving 
           all of the user registers */
        sd t0, 112(a0)

        /* Initialize kernel stack pointer, remember each process 
        has a kernel stack, the proc->kstack contains the 
        virtual address of the kernel stack */
        ld sp, 8(a0)

        /* Load the address of usertrap(), this should be set before 
        returning to user mode */
        ld t0, 16(a0)

        /* load tp */
        ld tp, 32(a0)

        /* save PC */
        csrr t2, sepc
        sd t2, 24(a0) 

        /* Wait for any previous memory operations to complete 
        which uses the user page table */
        sfence.vma zero, zero

        /* Install the kernel page table */
        ld t1, 0(a0)
        csrw satp, t1

        /* Flush now-stale user entries from the TLB */
        sfence.vma zero, zero

        /* Jump to usertrap(), which does not return */
        /* NOTICE all memory access to the trap frame is done before switching 
           the page table */
        jr t0

.globl userret
userret:
        # userret(pagetable)
        # called by usertrapret() in trap.c to
        # switch from kernel to user.
        # a0: user page table, for satp.

        # switch to the user page table.
        sfence.vma zero, zero
        csrw satp, a0
        sfence.vma zero, zero

        li a0, 0x3fffffe000

        # restore all but a0 from TRAPFRAME
        ld ra, 40(a0)
        ld sp, 48(a0)
        ld gp, 56(a0)
        ld tp, 64(a0)
        ld t0, 72(a0)
        ld t1, 80(a0)
        ld t2, 88(a0)
        ld s0, 96(a0)
        ld s1, 104(a0)
        ld a1, 120(a0)
        ld a2, 128(a0)
        ld a3, 136(a0)
        ld a4, 144(a0)
        ld a5, 152(a0)
        ld a6, 160(a0)
        ld a7, 168(a0)
        ld s2, 176(a0)
        ld s3, 184(a0)
        ld s4, 192(a0)
        ld s5, 200(a0)
        ld s6, 208(a0)
        ld s7, 216(a0)
        ld s8, 224(a0)
        ld s9, 232(a0)
        ld s10, 240(a0)
        ld s11, 248(a0)
        ld t3, 256(a0)
        ld t4, 264(a0)
        ld t5, 272(a0)
        ld t6, 280(a0)

	# restore user a0
        ld a0, 112(a0)
        
        # return to user mode and user pc.
        # usertrapret() set up sstatus and sepc.
        sret
