/*
 * proc.c - Processes management
 *
 * This file contains the implementation of core management functions
 * and data structures for managing cores in a multi-core system.
 */
#include "../include/proc.h"
#include "../include/riscv.h"
#include "../include/printk.h"
#include "../include/spinlock.h"
#include "../include/kmalloc.h"
#include "../include/string.h"
#include "../include/types.h"
#include "../include/kerror.h"
#include "../include/vm.h"

pid_t current_pid = 1;
struct spinlock pid_lock;

core_t cores[NCORE];
proc_t procs[NPROC];

extern char trap[];
extern ptb_t kernel_ptb;

/* Read tp register to get current core ID */
int get_coreid(){
  return read_tp();
}

/* Get current core structure */
struct core* get_mycore(){
  int id = get_coreid();
  return &cores[id];
}

// FIXME
struct proc*
get_myproc(void)
{
  intr_push();
  struct core *c = get_mycore();
  struct proc *p = c->proc;
  intr_pop();
  return p;
}

/* Get next valid pid */
pid_t next_pid(){
  pid_t res = 0;
  acquire_spinlock(&pid_lock);
  res = current_pid;
  current_pid++;
  release_spinlock(&pid_lock);
  return res;
}

/* initialize proc structures */
void proc_init(){
  printk("+------------------------------------------+\n");
  printk("|               proc_init                  |\n");
  printk("+------------------------------------------+\n");
  void *new_kstack = 0;

  /* map kernel stack */
  for (int i=0; i<NPROC; i++){
    spinlock_init(&procs[i].lock);
    procs[i].state  = INITED;
    procs[i].kstack = GET_PROC_KSTACK(i); /* VA for the stack page */
    if ((new_kstack = kmalloc()) == 0) kerror(__FILE_NAME__,__LINE__,"Error in proc_init kstack");
    if(!map_pages(kernel_ptb, 
              procs[i].kstack, 
              PSIZE, 
              (uint64_t)new_kstack, 
              PTE_R|PTE_W|PTE_X, 
              "kernel stacks")){
                kerror(__FILE_NAME__,__LINE__,"Error in mapping Kernel stack");
              }
  }
}

void restore_proc(proc_t* proc){
  if(proc->trapframe) kfree((void*)proc->trapframe);
  // if(proc->pagetable) proc_freepagetable(p->pagetable, p->sz); todo
  proc->state     = INITED;
  proc->trapframe = 0;
  proc->pagetable = 0;
  proc->name[0]   = 0;
  proc->parent    = 0;
  proc->killed    = 0;
  proc->xstate    = 0;
  proc->chan      = 0;
  proc->pid       = 0;
  proc->sz        = 0;
}

int prep_trap_frame(proc_t* proc){
  struct trapframe *new_tf = (struct trapframe*)kmalloc();
  if (new_tf == 0) return 0;

  proc->trapframe = new_tf;
  return 1;
}

int prep_page_table(proc_t* proc){
  ptb_t pagetable = (ptb_t)kmalloc();
  if(pagetable == 0) return 0;
  memset(pagetable, 0, PSIZE);

  if (!map_pages(pagetable, TRAP, PSIZE, (uint64_t)trap, PTE_R | PTE_X, "utrap")){
    printk("prep_page_table error when mapping trap.\n");
    kerror(__FILE_NAME__,__LINE__,"Error prep_page_table");
    // unmap_pages(pagetable, TRAP, PSIZE);
    // TODO
    return 0;
  }

  if (!map_pages(pagetable, TRAP_FRAME, PSIZE, (uint64_t) proc->trapframe, PTE_R | PTE_W, "utrap_frame")){
    printk("prep_page_table error when mapping utrap_frame.\n");
    kerror(__FILE_NAME__,__LINE__,"Error prep_page_table");
    // TODO
    return 0;
  }
  return 1;
}

/* Return an inited process structure for using, now it 
  should be empty */
proc_t* get_new_proc(){
  proc_t* proc = 0;

  for (int i=0; i<NPROC; i++){
    proc = procs + i;
    acquire_spinlock(&proc->lock);
    if (proc->state == INITED){ // return this
      /* prepare trap frame and page table */
      if(!prep_trap_frame(proc) || !prep_page_table(proc)) {
        restore_proc(proc);
        release_spinlock(&proc->lock);
        return 0;
      }
      memset(&proc->context, 0, sizeof(proc->context));
      // proc->context.ra = (uint64_t)forkret;
      proc->context.sp = proc->kstack + PSIZE;

      proc->state = PICKED;
      proc->pid = next_pid();
      return proc;
    } else {
      release_spinlock(&proc->lock);
    }
  }
  return 0;
}

void start_proc(){
  

}