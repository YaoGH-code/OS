#ifndef _buf_h_
#define _buf_h_

#include "types.h"
#include "sleeplock.h"
#include "fs.h"

#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache

struct buf {
  int valid;   // has data been read from disk?
  int disk;    // does disk "own" buf?
  uint32_t dev;
  uint32_t blockno;
  struct sleeplock lock;
  uint32_t refcnt;
  struct buf *prev; // LRU cache list
  struct buf *next;
  uint8_t data[BLOCK_SIZE];
};

#endif
