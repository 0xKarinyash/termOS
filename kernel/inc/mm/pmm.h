// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#ifndef PMM_H
#define PMM_H

#include "bootinfo.h"

#define PAGE_SIZE 4096
#define BLOCKS_PER_BYTE 8

#define SAFE_SPACE_START_ADDR 0x100000

#define BITMAP_BYTE_INDEX(addr)  ((addr / PAGE_SIZE) / BLOCKS_PER_BYTE)
#define BITMAP_BIT_OFFSET(addr)  ((addr / PAGE_SIZE) % BLOCKS_PER_BYTE)
#define BITMAP_TEST(bitmap, addr)  (bitmap[BITMAP_BYTE_INDEX(addr)] & (1 << BITMAP_BIT_OFFSET(addr)))
#define BITMAP_SET(bitmap, addr)   (bitmap[BITMAP_BYTE_INDEX(addr)] |= (1 << BITMAP_BIT_OFFSET(addr)))
#define BITMAP_UNSET(bitmap, addr) (bitmap[BITMAP_BYTE_INDEX(addr)] &= ~(1 << BITMAP_BIT_OFFSET(addr)))

void  pmm_init(BI_MemoryMap mmap);
void* pmm_alloc_page();
void  pmm_free_page(void* addr);


#endif