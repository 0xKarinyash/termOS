// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

#define KERNEL_HEAP_START   0xFFFFFFFFC0000000
#define PHYS_TO_HEAP(phys) ((phys) + KERNEL_HEAP_START)
#define HEAP_TO_PHYS(phys) ((phys) - KERNEL_HEAP_START)

#define HEADER_MAGIC 0x1CE

typedef struct block_header {
    u64 magic;
    struct block_header* next;
    struct block_header* prev;
    u64 size;
    bool is_free;
} block_header;

void heap_init();
void* malloc(u64 size);
void free(void* ptr);
void* realloc(void* ptr, u64 new_size);
