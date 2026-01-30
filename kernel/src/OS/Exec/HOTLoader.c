// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <OS/Exec/HOTLoader.h>
#include <OS/OSScheduler.h>
#include <lib/String.h>

#include <VM/PMM.h>
#include <VM/VMM.h>
#include <VM/Heap.h>

#include "../../common/hot_header.h"

UInt64 HOTLoad(OSProcess* process, UInt8* data) {
    HOTHeader* header = (HOTHeader*)data;
    if (header->magic != HOT_MAGIC) {
        return -1;
    }

    HOTSegment* segments = (HOTSegment*)(data + sizeof(HOTHeader));
    for (UInt64 i = 0; i < header->segments_count; i++) {
        HOTSegment* segment = &segments[i];
        if (segment->memsz == 0) continue;

        UInt64 start = segment->vaddr & ~(0xFFFULL);
        UInt64 end = (segment->vaddr + segment->memsz + 0xFFF) & ~(0xFFFULL);

        for (UInt64 address = start; address < end; address += kVMPageSize) {
            void* physicalPage = VMPhysicalMemoryAllocatePage();
            VMVirtualMemoryMapPage((UInt64*)process->physicalPML4, (UInt64)physicalPage, address, PTE_USER | PTE_RW | PTE_PRESENT);
            void* kernelVirtAddress = (void*)((UInt64)physicalPage + HHDM_OFFSET);
            memset(kernelVirtAddress, 0, kVMPageSize);
            UInt64 pageOverleapStart = (address > segment->vaddr) ? address : segment->vaddr;
            UInt64 pageOverleapEnd = (address + kVMPageSize < segment->vaddr + segment->filesz) 
                ? (address + kVMPageSize) 
                : (segment->vaddr + segment->filesz);
            if (pageOverleapStart < pageOverleapEnd) {
                UInt64 copySize = pageOverleapEnd - pageOverleapStart;
                UInt64 sourceOffset = segment->offset + (pageOverleapStart - segment->vaddr);
                UInt64 destinationOffset = pageOverleapStart - address;

                memcpy((UInt8*)kernelVirtAddress + destinationOffset, data + sourceOffset, copySize);
            }
        }
    }

    return header->entry_point;
}