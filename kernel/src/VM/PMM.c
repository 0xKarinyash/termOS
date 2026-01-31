// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <VM/PMM.h>
#include <VM/VMM.h>
#include <lib/String.h>
#include <lib/Math.h>

#include <OS/OSPanic.h>
#include <OS/OSSpinlock.h>

#include <types.h>

#include "bootinfo.h"


UInt8*  gVMPhycalMemoryBitmap = nullptr;
UInt64  gVMPhycalMemoryBitmapSize = 0;
UInt64  gVMPhycalMemoryTotalMemorySize = 0;

static OSSpinlock sVMPMMLock = {0};

extern UInt64 _kernel_start; 
extern UInt64 _kernel_end;

UInt64 VMPhysicalMemoryGetTotalMemorySize() {
    return gVMPhycalMemoryTotalMemorySize;
}

void VMPhysicalMemoryInitialize(BIMemoryMap *memoryMap) {
    UInt64 descriptorCount = memoryMap->mapSize / memoryMap->descriptorSize;
    UInt64 maxPhysicalAddress = 0;

    for (UInt64 i = 0; i < descriptorCount; i++) {
        OSMemoryDescriptor* descriptor = (OSMemoryDescriptor*)((UInt8*)memoryMap->map + (i * memoryMap->descriptorSize));
        
        if (descriptor->type == kOSMemoryTypeMappedIO || 
            descriptor->type == kOSMemoryTypeMappedIOPortSpace || 
            descriptor->type == kOSMemoryTypeUnusable || 
            descriptor->type == kOSMemoryTypeReserved || 
            descriptor->type == kOSMemoryTypePalCode) {
            continue;
        }

        UInt64 nominee = descriptor->physicalStart + (descriptor->pageCount * kVMPageSize);
        if (nominee > maxPhysicalAddress) {
            maxPhysicalAddress = nominee;
        }
    }
    gVMPhycalMemoryTotalMemorySize = maxPhysicalAddress;

    UInt64 totalPageCount = maxPhysicalAddress / kVMPageSize;
    UInt64 bitmapSize = (totalPageCount + 7) / 8;
    OSMemoryDescriptor *bitmapHostDescriptor = nullptr;

    for (UInt64 i = 0; i < descriptorCount; i++) {
        OSMemoryDescriptor *descriptor = (OSMemoryDescriptor*)((UInt8*)memoryMap->map + (i * memoryMap->descriptorSize));
        
        if ((descriptor->type == kOSMemoryTypeConventional) && 
            ((descriptor->pageCount * kVMPageSize) >= bitmapSize) && 
            (descriptor->physicalStart >= kVMSafeSpaceStartAddress)) { 
                bitmapHostDescriptor = descriptor;
                break;
        }
    }

    if (bitmapHostDescriptor == nullptr) {
        OSPanic("PMM: Not enough RAM for memory bitmap!");
    }

    gVMPhycalMemoryBitmap = (UInt8*)bitmapHostDescriptor->physicalStart;
    gVMPhycalMemoryBitmapSize = bitmapSize; 

    MemorySet(gVMPhycalMemoryBitmap, 0xFF, bitmapSize);

    for (UInt64 i = 0; i < descriptorCount; i++) {
        OSMemoryDescriptor *descriptor = (OSMemoryDescriptor*)((UInt8*)memoryMap->map + (i * memoryMap->descriptorSize));
        
        if (descriptor->type != kOSMemoryTypeConventional) {
            continue;
        }
        
        UInt64 startAddress = descriptor->physicalStart;
        UInt64 endAddress = startAddress + (descriptor->pageCount * kVMPageSize);
        
        for (UInt64 addr = startAddress; addr < endAddress; addr += kVMPageSize) {
            BITMAP_UNSET(gVMPhycalMemoryBitmap, addr);
        }
    }
    
    UInt64 kernelStart = KERNEL_VIRT_TO_PHYS((UInt64)&_kernel_start); 
    UInt64 kernelEnd   = KERNEL_VIRT_TO_PHYS((UInt64)&_kernel_end);

    UInt64 bitmapStart = (UInt64)gVMPhycalMemoryBitmap;
    UInt64 bitmapEnd   = bitmapStart + gVMPhycalMemoryBitmapSize;

    for (UInt64 addr = 0; addr < kVMSafeSpaceStartAddress; addr += kVMPageSize) BITMAP_SET(gVMPhycalMemoryBitmap, addr);
    for (UInt64 addr = kernelStart; addr < kernelEnd; addr += kVMPageSize)     BITMAP_SET(gVMPhycalMemoryBitmap, addr); 
    for (UInt64 addr = bitmapStart; addr < bitmapEnd; addr += kVMPageSize)     BITMAP_SET(gVMPhycalMemoryBitmap, addr);
}

void* VMPhysicalMemoryAllocatePage() {
    OSSpinlockState state;
    OSSpinlockLockIRQ(&sVMPMMLock, &state);

    for (UInt64 i = 0; i < gVMPhycalMemoryBitmapSize; i++) {
        if (gVMPhycalMemoryBitmap[i] == 0xFF) continue;
        
        for (UInt64 bit = 0; bit < 8; bit++) {
            if ((gVMPhycalMemoryBitmap[i] & (1 << bit)) == 0) { 
                UInt64 address = (i * 8 + bit) * kVMPageSize;
                BITMAP_SET(gVMPhycalMemoryBitmap, address);

                OSSpinlockUnlockIRQ(&sVMPMMLock, &state);
                return (void*)address;
            }
        }
    }

    OSSpinlockUnlockIRQ(&sVMPMMLock, &state);
    return nullptr; // Out of memory
}

void VMPhysicalMemoryFreePage(void* address) {
    OSSpinlockState state;
    OSSpinlockLockIRQ(&sVMPMMLock, &state);

    BITMAP_UNSET(gVMPhycalMemoryBitmap, (UInt64)address);

    OSSpinlockUnlockIRQ(&sVMPMMLock, &state);
}