#ifndef _proc_h_
#define _proc_h_

#include "../include/types.h"
#include "../include/param.h"
#include "../include/spinlock.h"
#include "../include/vm.h"

#define pid_t int

enum proc_state { INITED, PICKED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct context {
  uint64_t ra;
  uint64_t sp;

  /* callee-saved */
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

struct trapframe {
  /*   0 */ uint64_t kernel_satp;   // kernel page table
  /*   8 */ uint64_t kernel_sp;     // top of process's kernel stack
  /*  16 */ uint64_t kernel_trap;   // usertrap()
  /*  24 */ uint64_t epc;           // saved user program counter
  /*  32 */ uint64_t kernel_hartid; // saved kernel tp
  /*  40 */ uint64_t ra;
  /*  48 */ uint64_t sp;
  /*  56 */ uint64_t gp;
  /*  64 */ uint64_t tp;
  /*  72 */ uint64_t t0;
  /*  80 */ uint64_t t1;
  /*  88 */ uint64_t t2;
  /*  96 */ uint64_t s0;
  /* 104 */ uint64_t s1;
  /* 112 */ uint64_t a0;
  /* 120 */ uint64_t a1;
  /* 128 */ uint64_t a2;
  /* 136 */ uint64_t a3;
  /* 144 */ uint64_t a4;
  /* 152 */ uint64_t a5;
  /* 160 */ uint64_t a6;
  /* 168 */ uint64_t a7;
  /* 176 */ uint64_t s2;
  /* 184 */ uint64_t s3;
  /* 192 */ uint64_t s4;
  /* 200 */ uint64_t s5;
  /* 208 */ uint64_t s6;
  /* 216 */ uint64_t s7;
  /* 224 */ uint64_t s8;
  /* 232 */ uint64_t s9;
  /* 240 */ uint64_t s10;
  /* 248 */ uint64_t s11;
  /* 256 */ uint64_t t3;
  /* 264 */ uint64_t t4;
  /* 272 */ uint64_t t5;
  /* 280 */ uint64_t t6;
};

/* Per-process state */
typedef struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:
  enum proc_state state;        // Process state
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID

  // wait_lock must be held when using this:
  struct proc *parent;         // Parent process

  // these are private to the process, so p->lock need not be held.
  uint64_t kstack;               // Virtual address of kernel stack
  uint64_t sz;                   // Size of process memory (bytes)
  ptb_t pagetable;             // User page table
  struct trapframe *trapframe; // data page for trampoline.S
  struct context context;      // swtch() here to run process
  // struct file *ofile[NOFILE];  // Open files TODO
  // struct inode *cwd;           // Current directory TODO
  char name[16];               // Process name (debugging)
}proc_t;


/* Per-Core state */
typedef struct core {
  struct proc *proc;
  struct context context; 
  int disable_cnt;                
  int prev_int_state;        
}core_t;

extern struct core cores[NCORE];

int get_coreid();
struct core* get_mycore();
struct proc* get_myproc();
pid_t next_pid();
void proc_init();
void restore_proc(proc_t* proc);
int prep_trap_frame(proc_t* proc);
int prep_page_table(proc_t* proc);
struct proc* get_new_proc();
void start_proc();

#endif