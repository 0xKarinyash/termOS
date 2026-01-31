// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <VM/Heap.h>
#include <VM/PMM.h>
#include <VM/VMM.h>
#include <lib/String.h>
#include <OS/OSPanic.h>
#include <OS/OSSpinlock.h>
#include <types.h>

extern UInt64* gVMKernelPML4;
static VMHeapBlockHeader* sVMHeapListHead = nullptr;
static OSSpinlock sVMHeapLock = {0};

static void sVMHeapCombineForward(VMHeapBlockHeader* current) {
    if (!current->next || !current->next->isFree) return;
    current->size += sizeof(VMHeapBlockHeader) + current->next->size;
    current->next = current->next->next;
    if (current->next) current->next->previous = current; // what the fuck
}

void VMHeapInitialize() {
    UInt64 heapStart = kVMKernelHeapStart;

    for (UInt64 i = 0; i < kVMHeapSizePages; i++) {
        UInt64 physical = (UInt64)VMPhysicalMemoryAllocatePage();
        if (!physical) OSPanic("OOM during heap init");

        UInt64 virtual = heapStart + (i * kVMPageSize);
        VMVirtualMemoryMapPage(gVMKernelPML4, physical, virtual, PTE_PRESENT | PTE_RW);
    }

    sVMHeapListHead = (VMHeapBlockHeader*)heapStart;
    sVMHeapListHead->magic = kVMHeapBlockHeaderMagic;
    sVMHeapListHead->size = (kVMHeapSizePages * kVMPageSize) - sizeof(VMHeapBlockHeader);
    sVMHeapListHead->isFree = true;
    sVMHeapListHead->next = nullptr;
    sVMHeapListHead->previous = nullptr;
}

static void* sVMHeapAllocateInternal(UInt64 size) {
    if (size == 0) return nullptr;
    UInt64 alignedSize = (size + 15) & ~15;

    VMHeapBlockHeader* current = sVMHeapListHead;
    while (current) {
        if (current->isFree && current->size >= alignedSize) {
            if (current->size > alignedSize + sizeof(VMHeapBlockHeader) + 16) {
                VMHeapBlockHeader* new_block = (VMHeapBlockHeader*)((UInt64)current + sizeof(VMHeapBlockHeader) + alignedSize);
                new_block->size = current->size - alignedSize - sizeof(VMHeapBlockHeader);
                new_block->isFree = true;
                new_block->next = current->next;
                new_block->previous = current;
                new_block->magic = kVMHeapBlockHeaderMagic;
                
                if (current->next) current->next->previous = new_block;
                current->next = new_block;
                current->size = alignedSize;
            }
            current->isFree = false;
            return (void*)((UInt64)current + sizeof(VMHeapBlockHeader));
        }
        current = current->next;
    }

    return nullptr;
}

static void sVMHeapFreeInternal(void* pointer) {
    if (!pointer) return;

    VMHeapBlockHeader* current = (VMHeapBlockHeader*)((UInt64)pointer - sizeof(VMHeapBlockHeader));
    if (current->magic != kVMHeapBlockHeaderMagic) return;
    
    current->isFree = true;
    if (current->next && current->next->isFree) sVMHeapCombineForward(current);
    if (current->previous && current->previous->isFree) sVMHeapCombineForward(current->previous);
}

void* VMHeapAllocate(UInt64 size) {
    OSSpinlockState state;
    OSSpinlockLockIRQ(&sVMHeapLock, &state);
    
    void* result = sVMHeapAllocateInternal(size);
    
    OSSpinlockUnlockIRQ(&sVMHeapLock, &state);
    return result;
}

void VMHeapFree(void* pointer) {
    OSSpinlockState state;
    OSSpinlockLockIRQ(&sVMHeapLock, &state);
    
    sVMHeapFreeInternal(pointer);
    
    OSSpinlockUnlockIRQ(&sVMHeapLock, &state);
}

void* VMHeapResize(void* pointer, UInt64 newSize) {
    if (!pointer) return VMHeapAllocate(newSize);
    if (newSize == 0) { 
        VMHeapFree(pointer); 
        return nullptr; 
    }

    OSSpinlockState state;
    OSSpinlockLockIRQ(&sVMHeapLock, &state);

    VMHeapBlockHeader* current = (VMHeapBlockHeader*)((UInt64)pointer - sizeof(VMHeapBlockHeader));
    if (current->size >= newSize) { 
        OSSpinlockUnlockIRQ(&sVMHeapLock, &state);
        return pointer;
    }

    if (current->next && current->next->isFree &&
        (current->size + sizeof(VMHeapBlockHeader) + current->next->size) >= newSize) { // why ts so fucking unreadable
            sVMHeapCombineForward(current);
            OSSpinlockUnlockIRQ(&sVMHeapLock, &state);
            return pointer;
    }

    void* newPointer = sVMHeapAllocateInternal(newSize);
    if (newPointer) {
        MemoryCopy(newPointer, pointer, current->size);
        sVMHeapFreeInternal(pointer);
    }

    OSSpinlockUnlockIRQ(&sVMHeapLock, &state);
    return newPointer;
}