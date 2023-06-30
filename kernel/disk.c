/*
 * disk.c 
 * From: https://docs.oasis-open.org/virtio/virtio/v1.1/virtio-v1.1.pdf
 */

#include "../include/disk.h"
#include "../include/mmio.h"
#include "../include/kerror.h"
#include "../include/printk.h"
#include "../include/spinlock.h"
#include "../include/kmalloc.h"
#include "../include/string.h"


static struct disk {
  // a set (not a ring) of DMA descriptors, with which the
  // driver tells the device where to read and write individual
  // disk operations. there are NUM descriptors.
  // most commands consist of a "chain" (a linked list) of a couple of
  // these descriptors.
  struct virtq_desc *desc;

  // a ring in which the driver writes descriptor numbers
  // that the driver would like the device to process.  it only
  // includes the head descriptor of each chain. the ring has
  // NUM elements.
  struct virtq_avail *avail;

  // a ring in which the device writes descriptor numbers that
  // the device has finished processing (just the head of each chain).
  // there are NUM used ring entries.
  struct virtq_used *used;

  // our own book-keeping.
  char free[NUM];  // is a descriptor free?
  uint16_t used_idx; // we've looked this far in used[2..NUM].

  // track info about in-flight operations,
  // for use when completion interrupt arrives.
  // indexed by first descriptor index of chain.
  struct {
    struct buf *b;
    char status;
  } info[NUM];

  // disk command headers.
  // one-for-one with descriptors, for convenience.
  struct virtio_blk_req ops[NUM];
  struct spinlock vdisk_lock;
  
} disk;

void disk_init() {
    printk("+------------------------------------------+\n");
    printk("|               disk_init                  |\n");
    printk("+------------------------------------------+\n");
    /**
     * Device Initialization 
     * The driver MUST start the device initialization by reading and checking 
     * values from MagicValue and Version. If both values are valid, it MUST 
     * read DeviceID and if its value is zero (0x0) MUST abort initialization 
     * and MUST NOT access any other register.
     */
    if (mm_readw(VIRTIO_ADDR(VIRTIO_MAGIC_VALUE)) != 0x74726976 || 
        mm_readw(VIRTIO_ADDR(VIRTIO_VERSION))     != 2 ||
        mm_readw(VIRTIO_ADDR(VIRTIO_DEVICE_ID))   != 2 || 
        mm_readw(VIRTIO_ADDR(VIRTIO_VENDOR_ID))   != 0x554d4551) 
            kerror(__FILE_NAME__,__LINE__,"virtio disk not found");

    /**
     * Reset the device 
     * Reading from this register returns the current device status flags. 
     * Writing non-zero values to this register sets the status flags, 
     * indicating the driver progress. Writing zero (0x0) to this register 
     * triggers a device reset. See also p. 4.2.3.1 Device Initialization.      
     */
    mm_readw(VIRTIO_ADDR(VIRTIO_STATUS)) = 0;

    /** 
     * The OS has found the device and recognized it as a valid virtio 
     * device.
     */
    uint32_t status = STATUS_MSK_ACKNOWLEDGE;
    mm_writew(VIRTIO_ADDR(VIRTIO_STATUS), status);
    status |= STATUS_MSK_DRIVER;
    mm_writew(VIRTIO_ADDR((VIRTIO_STATUS)),status);

    /* Settting features with the device (picking the interection) */
    uint32_t features = mm_readw(VIRTIO_ADDR(VIRTIO_DEVICE_FEATURES));
    features &= ~(1 << BLK_FEATURE_BIT_RO);
    features &= ~(1 << BLK_FEATURE_BIT_SCSI);
    features &= ~(1 << BLK_FEATURE_BIT_CONFIG_WCE);
    features &= ~(1 << BLK_FEATURE_BIT_MQ);
    features &= ~(1 << QUEUE_FEATURE_BIT_ANY_LAYOUT);
    features &= ~(1 << QUEUE_FEATURE_BIT_EVENT_IDX);
    features &= ~(1 << QUEUE_FEATURE_BIT_INDIRECT_DESC);
    mm_writew((VIRTIO_ADDR(VIRTIO_DEVICE_FEATURES)),features);

    /* Features are OK */
    status |= STATUS_MSK_FEATURES_OK;
    mm_writew(VIRTIO_ADDR(VIRTIO_STATUS),status);
    if (!(mm_readw(VIRTIO_ADDR(VIRTIO_STATUS)) & STATUS_MSK_FEATURES_OK))
        kerror(__FILE_NAME__,__LINE__,"failed to set FEATURES_OK");
    
    /* Initialize queue 0 */
    mm_writew(VIRTIO_ADDR(VIRTIO_QUEUE_SEL),0);
    if(mm_readw(VIRTIO_ADDR(VIRTIO_QUEUE_READY)))
        kerror(__FILE_NAME__,__LINE__,"virtio queue not ready");
    
    /* check maximum queue size */
    uint32_t max = mm_readw(VIRTIO_ADDR(VIRTIO_QUEUE_SIZE_MAX));
    if(max == 0)
        kerror(__FILE_NAME__,__LINE__,"virtio has no queue 0");
    if(max < NUMDESC)
        kerror(__FILE_NAME__,__LINE__,"virtio max queue too short");

    disk.desc = kmalloc();
    disk.avail = kmalloc();
    disk.used = kmalloc();
    if(!disk.desc || !disk.avail || !disk.used)
        kerror(__FILE_NAME__,__LINE__,"virtio disk kalloc");
    memset(disk.desc, 0, PSIZE);
    memset(disk.avail, 0, PSIZE);
    memset(disk.used, 0, PSIZE);

    mm_writew(VIRTIO_ADDR(VIRTIO_QUEUE_SIZE),NUM);

    mm_writew(VIRTIO_ADDR(VIRTIO_DESC_LOW),(uint64_t)disk.desc);
    mm_writew(VIRTIO_ADDR(VIRTIO_DESC_HIGH),(uint64_t)disk.desc >> 32);
    mm_writew(VIRTIO_ADDR(VIRTIO_DRIVER_DESC_LOW),(uint64_t)disk.avail);
    mm_writew(VIRTIO_ADDR(VIRTIO_DRIVER_DESC_HIGH),(uint64_t)disk.avail >> 32);
    mm_writew(VIRTIO_ADDR(VIRTIO_DEVICE_DESC_LOW),(uint64_t)disk.used);
    mm_writew(VIRTIO_ADDR(VIRTIO_DEVICE_DESC_HIGH),(uint64_t)disk.used >> 32);

    mm_writew(VIRTIO_ADDR(VIRTIO_QUEUE_READY),0x1);

    for(int i = 0; i < NUM; i++)
        disk.free[i] = 1;

    status |= STATUS_MSK_DRIVER_OK;
    mm_writew(VIRTIO_ADDR(VIRTIO_STATUS),status);

    spinlock_init(&disk.vdisk_lock);
}

// find a free descriptor, mark it non-free, return its index.
static int alloc_desc(){
  for(int i = 0; i < NUM; i++){
    if(disk.free[i]){
      disk.free[i] = 0;
      return i;
    }
  }
  return -1;
}

// mark a descriptor as free.
static void free_desc(int i){
  if(i >= NUM)
    kerror(__FILE_NAME__,__LINE__,"free_desc 1");
  if(disk.free[i])
    kerror(__FILE_NAME__,__LINE__,"free_desc 2");
  disk.desc[i].addr = 0;
  disk.desc[i].len = 0;
  disk.desc[i].flags = 0;
  disk.desc[i].next = 0;
  disk.free[i] = 1;
//   wakeup(&disk.free[0]);
}

// // free a chain of descriptors.
// static void
// free_chain(int i)
// {
//   while(1){
//     int flag = disk.desc[i].flags;
//     int nxt = disk.desc[i].next;
//     free_desc(i);
//     if(flag & VRING_DESC_F_NEXT)
//       i = nxt;
//     else
//       break;
//   }
// }

// // allocate three descriptors (they need not be contiguous).
// // disk transfers always use three descriptors.
// static int
// alloc3_desc(int *idx)
// {
//   for(int i = 0; i < 3; i++){
//     idx[i] = alloc_desc();
//     if(idx[i] < 0){
//       for(int j = 0; j < i; j++)
//         free_desc(idx[j]);
//       return -1;
//     }
//   }
//   return 0;
// }

// void
// virtio_disk_rw(struct buf *b, int write)
// {
//   uint64 sector = b->blockno * (BSIZE / 512);

//   acquire(&disk.vdisk_lock);

//   // the spec's Section 5.2 says that legacy block operations use
//   // three descriptors: one for type/reserved/sector, one for the
//   // data, one for a 1-byte status result.

//   // allocate the three descriptors.
//   int idx[3];
//   while(1){
//     if(alloc3_desc(idx) == 0) {
//       break;
//     }
//     sleep(&disk.free[0], &disk.vdisk_lock);
//   }

//   // format the three descriptors.
//   // qemu's virtio-blk.c reads them.

//   struct virtio_blk_req *buf0 = &disk.ops[idx[0]];

//   if(write)
//     buf0->type = VIRTIO_BLK_T_OUT; // write the disk
//   else
//     buf0->type = VIRTIO_BLK_T_IN; // read the disk
//   buf0->reserved = 0;
//   buf0->sector = sector;

//   disk.desc[idx[0]].addr = (uint64) buf0;
//   disk.desc[idx[0]].len = sizeof(struct virtio_blk_req);
//   disk.desc[idx[0]].flags = VRING_DESC_F_NEXT;
//   disk.desc[idx[0]].next = idx[1];

//   disk.desc[idx[1]].addr = (uint64) b->data;
//   disk.desc[idx[1]].len = BSIZE;
//   if(write)
//     disk.desc[idx[1]].flags = 0; // device reads b->data
//   else
//     disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
//   disk.desc[idx[1]].flags |= VRING_DESC_F_NEXT;
//   disk.desc[idx[1]].next = idx[2];

//   disk.info[idx[0]].status = 0xff; // device writes 0 on success
//   disk.desc[idx[2]].addr = (uint64) &disk.info[idx[0]].status;
//   disk.desc[idx[2]].len = 1;
//   disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
//   disk.desc[idx[2]].next = 0;

//   // record struct buf for virtio_disk_intr().
//   b->disk = 1;
//   disk.info[idx[0]].b = b;

//   // tell the device the first index in our chain of descriptors.
//   disk.avail->ring[disk.avail->idx % NUM] = idx[0];

//   __sync_synchronize();

//   // tell the device another avail ring entry is available.
//   disk.avail->idx += 1; // not % NUM ...

//   __sync_synchronize();

//   *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number

//   // Wait for virtio_disk_intr() to say request has finished.
//   while(b->disk == 1) {
//     sleep(b, &disk.vdisk_lock);
//   }

//   disk.info[idx[0]].b = 0;
//   free_chain(idx[0]);

//   release(&disk.vdisk_lock);
// }

// void
// virtio_disk_intr()
// {
//   acquire(&disk.vdisk_lock);

//   // the device won't raise another interrupt until we tell it
//   // we've seen this interrupt, which the following line does.
//   // this may race with the device writing new entries to
//   // the "used" ring, in which case we may process the new
//   // completion entries in this interrupt, and have nothing to do
//   // in the next interrupt, which is harmless.
//   *R(VIRTIO_MMIO_INTERRUPT_ACK) = *R(VIRTIO_MMIO_INTERRUPT_STATUS) & 0x3;

//   __sync_synchronize();

//   // the device increments disk.used->idx when it
//   // adds an entry to the used ring.

//   while(disk.used_idx != disk.used->idx){
//     __sync_synchronize();
//     int id = disk.used->ring[disk.used_idx % NUM].id;

//     if(disk.info[id].status != 0)
//       panic("virtio_disk_intr status");

//     struct buf *b = disk.info[id].b;
//     b->disk = 0;   // disk is done with buf
//     wakeup(b);

//     disk.used_idx += 1;
//   }

//   release(&disk.vdisk_lock);
// }



void disk_isr(){
  
}