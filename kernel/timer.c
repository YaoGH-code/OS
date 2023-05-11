#include "../include/timer.h"
#include "../include/riscv.h"
#include "../include/types.h"
#include "../include/param.h"

uint64_t scratch[NCPU][5];

/* 
Timer interrupts come from clock hardware attached to each 
RISC-V CPU. OS programs this clock hardware to interrupt 
each CPU periodically. 

RISC-V requires that timer interrupts be taken in machine mode
, not supervisor mode.
*/
void timer_init(){
    int cpu_id = read_mhartid();
    /* Platforms provide a real-time counter, exposed as a 
    memory-mapped machine-mode register, mtime. mtime must run 
    at constant frequency, and the platform must provide a 
    mechanism for determining the timebase of mtime.
    The mtime register has a 64-bit precision on all RV32, RV64, 
    and RV128 systems. Platforms provide a 64-bit memory-mapped 
    machine-mode timer compare register (mtimecmp), which causes 
    a timer interrupt to be posted when the mtime register 
    contains a value greater than or equal to the value in the 
    mtimecmp register. The interrupt remains posted until it is 
    cleared by writing the mtimecmp register. The interrupt will 
    only be taken if interrupts are enabled and the MTIE bit is 
    set in the mie register. */
    uint64_t volatile mtime = *(uint64_t*) CLINT_MTIME;
    *(uint64_t*)CLINT_MTIMECMP(cpu_id) = mtime + TIMER_INTERVAL;

    /* The mscratch register is an XLEN-bit read/write register 
    dedicated for use by machine mode. Typically, it is used to 
    hold a pointer to a machine-mode hart-local context space and 
    swapped with a user register upon entry to an M-mode trap 
    handler. */
    scratch[cpu_id][3] = CLINT_MTIMECMP(cpu_id);
    scratch[cpu_id][4] = TIMER_INTERVAL;
    write_mscratch((uint64_t)scratch[cpu_id]);
}