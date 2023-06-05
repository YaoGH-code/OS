/*
 * proc.c - Processes management
 *
 * This file contains the implementation of core management functions
 * and data structures for managing cores in a multi-core system.
 */
#include "../include/proc.h"
#include "../include/riscv.h"
#include "../include/printk.h"

core_t cores[NCORE];
proc_t procs[NPROC];

/* Must be called with interrupts disabled, 
to prevent race with process being moved to 
a different CPU. */
int get_coreid(){
  return read_tp();
}

struct core* get_mycore(){
  int id = get_coreid();
  return &cores[id];
}

void proc_init(){
  printk("+------------------------------------------+\n");
  printk("|               proc_init                  |\n");
  printk("+------------------------------------------+\n");
  proc_t* p = 0;

  for (int i=0; i<NPROC; i++){
    p = &procs[i];
    spinlock_init(&p->lock);
    p->state = INITED;
    p->kstack = GET_PROC_KSTACK(i);
  }
}