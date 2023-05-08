/* The reason it places the kernel at 0x80000000 rather than 0x0 is because 
the address range 0x0:0x80000000 contains I/O devices */
.section .text
.global _entry

_entry:                 /* basically adding a offset ((mhartid+1)*4096) to 
                        find the starting address of each hart */
    la sp, stack0       # load the address of symbol stack0 to sp register
    li a0, 4096         # load the immediate to a0 register
    /*  csrr: Control and Status Register Read
    The mhartid CSR is an XLEN-bit read-only register containing the integer 
    ID of the hardware thread running the code. This register must be 
    readable in any implementation. Hart IDs might not necessarily be 
    numbered contiguously in a multiprocessor system, but at least one hart 
    must have a hart ID of zero. 
    mhartid -> Machine Hart ID -> to identify 
    currently running thread Hart -> Hardware Thread 
    In certain cases, we must ensure exactly one hart runs some code 
    (e.g., at reset), and so require one hart to have a known hart ID of zero. 
    For efficiency, system implementers should aim to reduce the magnitude of 
    the largest hart ID used in a system. */
    csrr a1, mhartid
    addi a1, a1, 1      # a1 += 1
    mul a0, a0, a1      # a0 *= a1
    add sp, sp, a0      # sp += a0
    call start          # go to start in start.c

