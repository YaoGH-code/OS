#ifndef _trap_handle_h_
#define _trap_handle_h_

/* interrupt cause */
#define CAUSE_EXTI 9
#define CAUSE_SOFT 1


/* Not interrupt cause */
#define CAUSE_ENV_CALL 8

#define IS_INTERRUPT(cause) ((cause) &  (1L<<63))
#define GET_EXP_CODE(cause) ((cause) & ~(1L<<63))

void user_trap();
void kernel_trap();
void trap_init();

#endif