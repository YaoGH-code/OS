#ifndef _file_h_
#define _file_h_

#include "types.h"
#include "sleeplock.h"
#include "fs.h"


// in-memory copy of an inode
struct inode {
  uint32_t dev;           // Device number
  uint32_t inum;          // Inode number
  int ref;            // Reference count
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint32_t size;
  uint32_t addrs[NDIRECT+1];
};

#endif