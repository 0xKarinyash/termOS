// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <Types.h>

enum { 
    kMemoryBlockMagic = 0x1CE1CE,
    kMemoryAlignment = 16,
    kMemoryPageSize = 4096
};

typedef struct MemoryBlockHeader {
    UInt64 magic;
    struct MemoryBlockHeader* next;
    struct MemoryBlockHeader* previous;
    UInt64 size;
    bool isFree;
} MemoryBlockHeader;

void* MemoryAllocate(UInt64 size);
void MemoryFree(void* pointer);
void* MemoryReallocate(void* pointer, UInt64 newSize);