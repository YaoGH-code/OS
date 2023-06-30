
#include "../include/disk.h"
#include "../include/mmio.h"
#include "../include/kerror.h"
#include "../include/printk.h"
#include "../include/spinlock.h"
#include "../include/kmalloc.h"
#include "../include/string.h"
#include "../include/file.h"
#include "../include/fs.h"

struct {
  struct spinlock lock;
  struct inode inode[NINODE];
} itable;

void
iinit()
{
  int i = 0;
  
  spinlock_init(&itable.lock);
  for(i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].lock, "inode");
  }
}