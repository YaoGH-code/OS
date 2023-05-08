#ifndef _hart_h_
#define _hart_h_

#include "types.h"

#define MSTATUS_MPP_MASK (3L << 11)
#define MSTATUS_MPP_MACHINE (3L << 11)
#define MSTATUS_MPP_SUPER (1L << 11)
#define MSTATUS_MPP_USER (0L << 11)

#define PMPXCFG_R (1)
#define PMPXCFG_W (1 << 1)
#define PMPXCFG_X (1 << 2)
#define PMPXCFG_A_TOR (1 << 3)
#define PMPXCFG_L (1 << 7)

// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software


#define FUNC_READ_CSR(register_name) \
static inline uint64_t \
read_##register_name() { \
    uint64_t val; \
    asm volatile("csrr %0," #register_name : "=r" (val) ); \
    return val; \
}

#define FUNC_WRITE_CSR(register_name) \
static inline void \
write_##register_name(uint64_t val) { \
    asm volatile("csrw " #register_name ", %0" :: "r" (val) ); \
}

#define FUNC_READ_GP(register_name) \
static inline uint64_t \
read_##register_name() { \
    uint64_t val; \
    asm volatile("mv %0," #register_name : "=r" (val) ); \
    return val; \
}

#define FUNC_WRITE_GP(register_name) \
static inline void \
write_##register_name(uint64_t val) { \
    asm volatile("mv " #register_name ", %0" :: "r" (val) ); \
}

FUNC_READ_CSR(mhartid)
FUNC_READ_CSR(mscratch)
FUNC_READ_CSR(mtvec)
FUNC_READ_CSR(mstatus)
FUNC_READ_CSR(mie)
FUNC_READ_CSR(mepc)
FUNC_READ_CSR(pmpaddr0)
FUNC_READ_CSR(pmpcfg0)
FUNC_READ_CSR(medeleg)
FUNC_READ_CSR(mideleg)
FUNC_READ_CSR(satp)
FUNC_READ_CSR(scause)
FUNC_READ_CSR(sie)
FUNC_READ_CSR(sstatus)
FUNC_READ_CSR(stvec)

FUNC_WRITE_CSR(mscratch)
FUNC_WRITE_CSR(mtvec)
FUNC_WRITE_CSR(mstatus)
FUNC_WRITE_CSR(mie)
FUNC_WRITE_CSR(mepc)
FUNC_WRITE_CSR(pmpaddr0)
FUNC_WRITE_CSR(pmpcfg0)
FUNC_WRITE_CSR(medeleg)
FUNC_WRITE_CSR(mideleg)
FUNC_WRITE_CSR(satp)
FUNC_WRITE_CSR(scause)
FUNC_WRITE_CSR(sie)
FUNC_WRITE_CSR(sstatus)
FUNC_WRITE_CSR(stvec)

FUNC_READ_GP(tp)
FUNC_READ_GP(sp)
FUNC_READ_GP(ra)

FUNC_WRITE_GP(tp)
FUNC_WRITE_GP(sp)
FUNC_WRITE_GP(ra)


#endif
