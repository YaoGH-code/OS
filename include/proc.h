#ifndef _proc_h_
#define _proc_h_

#include "../include/types.h"
#include "../include/param.h"

struct context {
  uint64_t ra;
  uint64_t sp;

  // callee-saved
  uint64_t s0;
  uint64_t s1;
  uint64_t s2;
  uint64_t s3;
  uint64_t s4;
  uint64_t s5;
  uint64_t s6;
  uint64_t s7;
  uint64_t s8;
  uint64_t s9;
  uint64_t s10;
  uint64_t s11;
};

// Per-CPU state.
struct core {
//   struct proc *proc;    // TODO     
  struct context context; 
  int disable_cnt;                
  int prev_int_state;        
};

extern struct core cores[NCORE];

int get_coreid();
struct core* get_mycore();

#endif