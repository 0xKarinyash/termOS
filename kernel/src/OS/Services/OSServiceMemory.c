// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <OS/Services/OSServiceMemory.h>

#include <OS/OSScheduler.h>

#include <VM/PMM.h>
#include <VM/VMM.h>
#include <lib/String.h>

UInt64 OSServiceMemoryAllocate(UInt64 size) {
    if (size == 0) return 0;
    OSProcess* currentProcess = gOSSchedulerCurrentTask->process;
    UInt64 addressToReturn = currentProcess->heapCurrentPointer;

    UInt64 pages_needed = (size + (kVMPageSize-1)) / kVMPageSize;

    for (UInt64 i = 0; i < pages_needed; i++) {
        void* phycialAddress = VMPhysicalMemoryAllocatePage();
        if (!phycialAddress) return 0;

        VMVirtualMemoryMapPage((UInt64*)currentProcess->physicalPML4, (UInt64)phycialAddress, currentProcess->heapCurrentPointer, PTE_PRESENT | PTE_RW | PTE_USER);
        memset((void*)PHYS_TO_HHDM((UInt64)phycialAddress), 0, kVMPageSize);

        currentProcess->heapCurrentPointer += kVMPageSize;
    }

    return addressToReturn;
}
