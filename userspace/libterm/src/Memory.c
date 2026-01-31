// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash
// brutally copypasting from kernel/mm/heap.c

#include <Memory.h>
#include <String.h>

static MemoryBlockHeader* sMemoryHeapListHead = nullptr;
extern UInt64 OSServiceMemoryAllocate(UInt64 size);

static void sMemoryCombineForward(MemoryBlockHeader* current) {
    if (!current->next || !current->next->isFree) return;
    current->size += sizeof(MemoryBlockHeader) + current->next->size;
    current->next = current->next->next;
    if (current->next) current->next->previous = current; 
}

void* MemoryAllocate(UInt64 size) {
    if (size == 0) return nullptr;
    UInt64 alignedSize = (size + (kMemoryAlignment-1)) & ~(kMemoryAlignment - 1);

    MemoryBlockHeader* current = sMemoryHeapListHead;
    MemoryBlockHeader* lastBlock = nullptr;

    while (current) {
        if (current->isFree && current->size >= alignedSize) {
            if (current->size > alignedSize + sizeof(MemoryBlockHeader) + kMemoryAlignment) {
                MemoryBlockHeader* newBlock = (MemoryBlockHeader*)((UInt64)current + sizeof(MemoryBlockHeader) + alignedSize);
                newBlock->size = current->size - alignedSize - sizeof(MemoryBlockHeader);
                newBlock->isFree = true;
                newBlock->next = current->next;
                newBlock->previous = current;
                newBlock->magic = kMemoryBlockMagic;
                
                if (current->next) current->next->previous = newBlock;
                current->next = newBlock;
                current->size = alignedSize;
            }
            current->isFree = false;
            return (void*)((UInt64)current + sizeof(MemoryBlockHeader));
        }
        lastBlock = current;
        current = current->next;
    }

    UInt64 sizeNeededToAllocate = alignedSize + sizeof(MemoryBlockHeader);
    
    UInt64 pageAlignedSize = (sizeNeededToAllocate + (kMemoryPageSize-1)) & ~(kMemoryPageSize-1);

    UInt64 newMemoryAddress = OSServiceMemoryAllocate(pageAlignedSize);
    if (newMemoryAddress == 0) return nullptr; 

    MemoryBlockHeader* newBlock = (MemoryBlockHeader*)newMemoryAddress;
    newBlock->size = pageAlignedSize - sizeof(MemoryBlockHeader);
    newBlock->isFree = true;
    newBlock->magic = kMemoryBlockMagic;
    newBlock->next = nullptr;
    newBlock->previous = lastBlock;

    if (lastBlock) {
        lastBlock->next = newBlock;
    } else {
        sMemoryHeapListHead = newBlock;
    }

    return MemoryAllocate(size);
}

void MemoryFree(void* pointer) {
    if (!pointer) return;

    MemoryBlockHeader* current = (MemoryBlockHeader*)((UInt64)pointer - sizeof(MemoryBlockHeader));
    if (current->magic != kMemoryBlockMagic) return;
    
    current->isFree = true;
    if (current->next && current->next->isFree) sMemoryCombineForward(current);
    if (current->previous && current->previous->isFree) sMemoryCombineForward(current->previous);
}

void* MemoryReallocate(void* pointer, UInt64 newSize) {
    if (!pointer) return MemoryAllocate(newSize);
    if (newSize == 0) { 
        MemoryFree(pointer); 
        return nullptr; 
    }

    MemoryBlockHeader* current = (MemoryBlockHeader*)((UInt64)pointer - sizeof(MemoryBlockHeader));
    if (current->size >= newSize) return pointer;

    if (current->next &&
        current->next->isFree &&
        (current->size + sizeof(MemoryBlockHeader) + current->next->size) >= newSize) { 
            sMemoryCombineForward(current);
            return pointer;
    }

    void* newPointer = MemoryAllocate(newSize);
    if (!newPointer) return nullptr;

    MemoryCopy(newPointer, pointer, current->size);
    MemoryFree(pointer);

    return newPointer;
}