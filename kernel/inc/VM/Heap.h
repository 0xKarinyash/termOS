// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

enum {
    kVMKernelHeapStart = 0xFFFFFFFFC0000000,
    kVMHeapSizePages = 1024
};

#define PHYS_TO_HEAP(phys) ((phys) + KERNEL_HEAP_START)
#define HEAP_TO_PHYS(phys) ((phys) - KERNEL_HEAP_START)

#define HEADER_MAGIC 0x1CE

typedef struct VMHeapBlockHeader {
    UInt64 magic;
    struct VMHeapBlockHeader* next;
    struct VMHeapBlockHeader* previous;
    UInt64 size;
    bool isFree;
} VMHeapBlockHeader;

void VMHeapInitialize();
void* malloc(UInt64 size);
void free(void* ptr);
void* realloc(void* ptr, UInt64 newSize);
