#ifndef _disk_h_
#define _disk_h_

#include "riscv.h"
#include "types.h"

#define VIRTIO_ADDR(offset) (VIRTIO + (offset))

/* MMIO VIRTIO Device Register Layout */
#define VIRTIO_MAGIC_VALUE          0x000
#define VIRTIO_VERSION              0x004
#define VIRTIO_DEVICE_ID            0x008
#define VIRTIO_VENDOR_ID            0x00c
#define VIRTIO_DEVICE_FEATURES      0x010
#define VIRTIO_DRIVER_FEATURES      0x020
#define VIRTIO_QUEUE_SEL            0x030 
#define VIRTIO_QUEUE_SIZE_MAX       0x034 
#define VIRTIO_QUEUE_SIZE           0x038 
#define VIRTIO_QUEUE_READY          0x044
#define VIRTIO_QUEUE_NOTIFY         0x050
#define VIRTIO_INTR_STATUS          0x060
#define VIRTIO_INTR_ACK             0x064
#define VIRTIO_STATUS               0x070
#define VIRTIO_DESC_LOW             0x080 
#define VIRTIO_DESC_HIGH            0x084
#define VIRTIO_DRIVER_DESC_LOW      0x090 
#define VIRTIO_DRIVER_DESC_HIGH     0x094
#define VIRTIO_DEVICE_DESC_LOW      0x0a0 
#define VIRTIO_DEVICE_DESC_HIGH     0x0a4
#define VIRTIO_MAGIC_NUM            0x74726976 // a Little Endian equivalent of the “virt” string

// status register bits, from qemu virtio_config.h
#define STATUS_MSK_ACKNOWLEDGE	    1
#define STATUS_MSK_DRIVER	    	2
#define STATUS_MSK_DRIVER_OK	    4
#define STATUS_MSK_FEATURES_OK      8

#define BLK_FEATURE_BIT_RO              5
#define BLK_FEATURE_BIT_SCSI            7
#define BLK_FEATURE_BIT_CONFIG_WCE      11
#define BLK_FEATURE_BIT_MQ              12
#define QUEUE_FEATURE_BIT_ANY_LAYOUT    27
#define QUEUE_FEATURE_BIT_INDIRECT_DESC 28
#define QUEUE_FEATURE_BIT_EVENT_IDX     29

#define NUMDESC 8 // number of descriptors in the table
#define NUM 8

struct virtq_desc {
  uint64_t addr;
  uint32_t len;
  uint16_t flags;
  uint16_t next;
};

// the (entire) avail ring, from the spec.
struct virtq_avail {
  uint16_t flags; // always zero
  uint16_t idx;   // driver will write ring[idx] next
  uint16_t ring[NUM]; // descriptor numbers of chain heads
  uint16_t unused;
};

// one entry in the "used" ring, with which the
// device tells the driver about completed requests.
struct virtq_used_elem {
  uint32_t id;   // index of start of completed descriptor chain
  uint32_t len;
};

struct virtq_used {
  uint16_t flags; // always zero
  uint16_t idx;   // device increments when it adds a ring[] entry
  struct virtq_used_elem ring[NUM];
};

// the format of the first descriptor in a disk request.
// to be followed by two more descriptors containing
// the block, and a one-byte status.
struct virtio_blk_req {
  uint32_t type; // VIRTIO_BLK_T_IN or ..._OUT
  uint32_t reserved;
  uint64_t sector;
};

void disk_init();
void disk_isr();
static int alloc_desc();



#endif