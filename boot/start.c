#include "../include/param.h"
#include "../include/riscv.h"
#include "../include/timer.h"

/* Size: 4096 * NCPU, each element 16B alignment */
__attribute__ ((aligned (16))) char stack0[4096 * NCORE];

void main();
void mti_handler();

/* entry.S jumps here in machine mode on stack0. */
void start()
{
    /* [x4 tp Thread pointer]
    Each CPU’s hartid is stored in that CPU’s tp register 
    while in the kernel. This allows mycpu to use tp to 
    index an array of cpu structures to find the right 
    one. 
    Store hartid to tp for later access from supervisor mode.
    */
    write_tp(read_mhartid());

    /* 
    ********************************************************
    *                                                      *
    *               About Privilege Levels                 *
    *                                                      *
    ********************************************************

    At any time, a RISC-V hardware thread (hart) is running at 
    some privilege level encoded as a mode in one or more CSRs 
    (control and status registers). Three RISC-V privilege 
    levels are currently defined: 
    ********************************************************
    * Level | Encoding | Abbreviation |        Name        *
    *-------|----------|--------------|--------------------*
    *   0   |    00    |      U       |  User/Application  *
    *   1   |    01    |      S       |     Supervisor     *
    *   2   |    10    |   Reserved   |                    *
    *   3   |    11    |      M       |       Machine      *
    ********************************************************

    The machine level has the highest privileges and is the 
    only mandatory privilege level for a RISC-V hardware 
    platform. Code run in machine-mode (M-mode) is usually 
    inherently trusted, as it has low-level access to the
    machine implementation. M-mode can be used to manage 
    secure execution environments on RISC-V. User-mode 
    (U-mode) and supervisor-mode (S-mode) are intended for 
    conventional application and operating system usage 
    respectively.

    [Machine Status Register (mstatus)]
    The mstatus register is an XLEN-bit read/write register
    The mstatus register keeps track of and controls the hart’s 
    current operating state. 

    MPP is Machine Previous Privilege. We first set MPP to supervisor 
    mode to let hardware know the "previous mode" is supervisor mode
    and we should "return" to supervisor mode when MRET is executed.
    This is a way to enter supervisor mode.

    So, here we are preparing previous privilege to be supervisor mode. */
    write_mstatus((read_mstatus()&~MSTATUS_MPP_MASK) | MSTATUS_MPP_SUPER);

    /* [Machine Exception Program Counter (mepc)]
    Set the "return" address to main by writing main’s address into 
    the register MEPC.
    MEPC -> Machine Exception Program Counter
    MRET will set PC to the value in the MEPC 
    So, we will "returning" to main at the end. */
    write_mepc((uint64_t)main);

    /* [Supervisor Address Translation and Protection (satp) Register]
    The satp register is an XLEN-bit read/write register, which controls 
    supervisor-mode address translation and protection. This register 
    holds the physical page number (PPN) of the root page table, i.e., 
    its supervisor physical address divided by 4 KiB; an address space 
    identifier (ASID), which facilitates address-translation fences on a 
    per-address-space basis; and the MODE field, which selects the current 
    address-translation scheme. 
    *****************************************
    *               RV64 satp               *
    *---------------------------------------*                
    * MODE (WARL) | ASID (WARL) | PPN (WARL)*
    *---------------------------------------*
    *     4       |     16      |     44    *
    *****************************************
    MODE: 0 Bare No translation or protection. */
    write_satp(0);

    /* [Physical Memory Protection CSRs]
    pmpaddr0 -> Physical memory protection address register. 
    **********************************************************************************
    *                         RV64 PMP configuration CSR layout                      *
    * pmp7cfg |pmp6cfg |pmp5cfg |pmp4cfg |pmp3cfg |pmp2cfg |pmp1cfg|pmp0cfg--pmpcfg0 *
    * pmp15cfg|pmp14cfg|pmp13cfg|pmp12cfg|pmp11cfg|pmp10cfg|pmp9cfg|pmp8cfg--pmpcfg2 *
    **********************************************************************************

    For RV64, pmpcfg0 and pmpcfg2 hold the configurations for the 16 PMP entries.
    The PMP address registers are CSRs named pmpaddr0–pmpaddr15.
    For RV64, each PMP address register encodes bits 55–2 of a 56-bit physical address.
    Here, we use write_pmpcfg0 to set A (WARL) X (WARL) W (WARL) R (WARL) to 1.
    A = 1 means TOR (Top of range). Also, if PMP entry 0’s A field is set to TOR, zero 
    is used for the lower bound, and so it matches any address a < pmpaddr0. 
    And, the top address is the highest address, so only one "segment" in physical
    memory. */
    write_pmpaddr0(0x3FFFFFFFFFFFFFULL);
    write_pmpcfg0((PMPXCFG_L | PMPXCFG_A_TOR | PMPXCFG_X | PMPXCFG_W | PMPXCFG_R));

    /* [Machine Trap Delegation Registers (medeleg and mideleg)]
    By default, all traps at any privilege level are handled in machine mode, though a 
    machine-mode handler can redirect traps back to the appropriate level with the MRET 
    instruction. To increase performance, implementations can provide individual read/
    write bits within medeleg and mideleg to indicate that certain exceptions and 
    interrupts should be processed directly by a lower privilege level. The machine 
    exception delegation register (medeleg) and machine interrupt delegation register 
    (mideleg) are XLEN-bit read/write registers. 

    The lower 16 bits of the register correspond to the 16 standard synchronous 
    exception types, while the upper 16 bits are reserved for custom exception types. 
    In systems with all three privilege modes (M/S/U), setting a bit in medeleg or 
    mideleg will delegate the corresponding trap in S-mode or U-mode to the S-mode 
    trap handler.
    */
    write_medeleg(0xFFFF);
    write_mideleg(0xFFFF);

    /* [Supervisor Interrupt Registers (sip and sie)]
    The sip register is an XLEN-bit read/write register containing information on pending 
    interrupts, while sie is the corresponding XLEN-bit read/write register containing 
    interrupt enable bits. 
    Turn external, timer and software interrupt on in supervisor mode.
    */
    write_sie(read_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

    /* Initialize the timer for all cores */
    timer_init();

    /* [Machine Trap-Vector Base-Address Register (mtvec)]
    *****************************************
    *               RV64 mtvec              *
    *---------------------------------------*                
    *  BASE[XLEN-1:2] (WARL) |  MODE (WARL) *
    *          XLEN-2               2       *
    *****************************************
    The mtvec register is an XLEN-bit read/write register that holds trap vector 
    configuration, consisting of a vector base address (BASE) and a vector mode (MODE).
    The value in the BASE field must always be aligned on a 4-byte boundary, and the 
    MODE setting may impose additional alignment constraints on the value in the BASE 
    field.
    *********************************************************************
    * Value |   Name   |                  Description                   *
    *-------|----------|------------------------------------------------*
    *   0   |  Direct  |         All exceptions set pc to BASE          *  
    *   1   | Vectored | Asynchronous interrupts set pc to BASE+4×cause *
    *  >=2  |    -—    |                   Reserved                     *
    *********************************************************************
    Since the base should be 4-byte aligned, the MODE is 0 by default.
    */
    write_mtvec((uint64_t)mti_handler);

    /* MIE */
    write_mstatus(read_mstatus() | MSTATUS_MIE);

    /* Machine interrupt-enable register -> timer interrrupt enable */
    write_mie(read_mie() | MIE_MTIE);

    /* return to main function in supervisor mode */
    asm volatile("mret");
}
