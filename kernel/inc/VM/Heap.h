// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>

enum {
    kVMKernelHeapStart = 0xFFFFFFFFC0000000,
    kVMHeapSizePages = 1024,
    kVMHeapBlockHeaderMagic = 0x1CE
};

#define PHYS_TO_HEAP(phys) ((phys) + KERNEL_HEAP_START)
#define HEAP_TO_PHYS(phys) ((phys) - KERNEL_HEAP_START)
typedef struct VMHeapBlockHeader {
    UInt64 magic;
    struct VMHeapBlockHeader* next;
    struct VMHeapBlockHeader* previous;
    UInt64 size;
    bool isFree;
} VMHeapBlockHeader;

void VMHeapInitialize();
void* VMHeapAllocate(UInt64 size);
void VMHeapFree(void* ptr);
void* VMHeapResize(void* ptr, UInt64 newSize);
