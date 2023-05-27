/*
 * core.c - Core management
 *
 * This file contains the implementation of core management functions
 * and data structures for managing cores in a multi-core system.
 */
#include "../include/proc.h"
#include "../include/riscv.h"

struct core cores[NCORE];

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