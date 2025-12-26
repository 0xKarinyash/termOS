#ifndef PMM_H
#define PMM_H

#include "types.h"
#include "../common/bootinfo.h"

#define PAGE_SIZE 4096
#define BLOCKS_PER_BYTE 8

#define BITMAP_BYTE_INDEX(addr)  ((addr / PAGE_SIZE) / BLOCKS_PER_BYTE)
#define BITMAP_BIT_OFFSET(addr)  ((addr / PAGE_SIZE) % BLOCKS_PER_BYTE)
#define BITMAP_TEST(bitmap, addr)  (bitmap[BITMAP_BYTE_INDEX(addr)] & (1 << BITMAP_BIT_OFFSET(addr)))
#define BITMAP_SET(bitmap, addr)   (bitmap[BITMAP_BYTE_INDEX(addr)] |= (1 << BITMAP_BIT_OFFSET(addr)))
#define BITMAP_UNSET(bitmap, addr) (bitmap[BITMAP_BYTE_INDEX(addr)] &= ~(1 << BITMAP_BIT_OFFSET(addr)))

void pmm_init(BI_MemoryMap mmap);
u8* get_bitmap();

#endif