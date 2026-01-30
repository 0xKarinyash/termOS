// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>

#define HEADER_MAGIC 0x1CE1CE

typedef struct block_header {
    u64 magic;
    struct block_header* next;
    struct block_header* prev;
    u64 size;
    bool is_free;
} block_header;

void* malloc(u64 size);
void free(void* ptr);
void* realloc(void* ptr, u64 new_size);