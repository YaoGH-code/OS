/* machine level timer interrupt handler */
.globl mti_handler
.align 4
mti_handler:
    /* Remember in timer.c, we but the address of scratch into 
    the mscratch register for each core.
    scratch[cpu_id][3] -> address of CLINT_MTIMECMP
    scratch[cpu_id][4] -> TIMER_INTERVAL
    Here, we will use four registers for this handler
    (a0,a1,a2,a3) */
    /* save prepared value in mscratch(address of scratch space) to 
    a0 and clear mscratch. */
    csrrw a0, mscratch, a0

    /* We can use scratch[cpu_id][0], scratch[cpu_id][1] and 
    scratch[cpu_id][2] which are empty before to save context. */
    sd a1, (a0)
    sd a2, 8(a0)
    sd a3, 16(a0)

    /* So, we could use a1 to store address of MTIMECMP */
    ld a1, 24(a0) # scratch[cpu_id][3] -> address of CLINT_MTIMECMP
    ld a2, 32(a0) # scratch[cpu_id][4] -> TIMER_INTERVAL

    /* Deferencing to get current value in MTIMECMP */
    ld a3, (a1)

    /* Compute and update next value for MTIMECMP */
    add a3, a2, a3 
    sw a3, (a1)

    /* A supervisor-level software interrupt is triggered on the 
    current hart by writing 1 to its supervisor software 
    interrupt-pending (SSIP) bit in the sip register. */
    csrw sip, 2

    /* Restore context */
    ld a1, (a0)
    ld a2, 8(a0)
    ld a3, 16(a0)
    csrrw a0, mscratch, a0

    mret
