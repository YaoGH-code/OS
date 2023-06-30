#ifndef _fs_h_
#define _fs_h_

#define BLOCK_SIZE 1024
#define NDIRECT    12
#define NINODE     50  // maximum number of active i-nodes

void iinit();

#endif