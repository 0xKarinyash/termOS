// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash
#include <VM/VMM.h>
#include <VM/PMM.h>

#include <OS/OSPanic.h>

#include <GDT.h>
#include <IDT.h>

#include <lib/String.h>

#include <types.h>
#include "bootinfo.h"

enum {
    kVMUserStackTop = 0x70000000,
    kVMUserStackSize = 0x4000
};

UInt64* gVMKernelPML4 = nullptr;
UInt64 gVMKernelPML4Physical = 0;
static bool isInitialized = false;

extern UInt64 _kernel_start; 
extern UInt64 _kernel_end;
extern UInt8* gVMPhycalMemoryBitmap;
extern UInt64 gVMPhycalMemoryBitmapSize;
extern HALGlobalDescriptorTable gHALGlobalDescriptorTable[];
extern HALInterruptsDescriptorTableEntry gHALInterruptsDescriptorTable[];
extern UInt8 gHALDoubleFaultStack[];
extern UInt8 stack_guard;

static UInt64* sVMGetVirtualTable(UInt64 phys) {
    if (isInitialized) return (UInt64*)PHYS_TO_HHDM(phys);
    return (UInt64*)phys;
}

UInt64* VMVirtualMemoryMapPage(UInt64* pml4, UInt64 phys, UInt64 virt, UInt64 flags) {
    UInt64 pt_idx = VMM_PT_INDEX(virt);
    UInt64 pd_idx = VMM_PD_INDEX(virt);
    UInt64 pdpt_idx = VMM_PDPT_INDEX(virt);
    UInt64 pml4_idx = VMM_PML4_INDEX(virt);

    UInt64* pml4_virt = pml4;
    if (isInitialized) pml4_virt = (UInt64*)PHYS_TO_HHDM((UInt64)pml4);

    UInt64 table_flags = PTE_PRESENT | PTE_RW | (flags & PTE_USER);

    if (!(pml4_virt[pml4_idx] & PTE_PRESENT)) {
        UInt64* addr = VMPhysicalMemoryAllocatePage();
        if (!addr) return nullptr;
        UInt64* addr_virt = sVMGetVirtualTable((UInt64)addr);
        memset(addr_virt, 0, kVMPageSize);
        pml4_virt[pml4_idx] = (UInt64)addr | table_flags; 
    } else {
        pml4_virt[pml4_idx] |= (flags & PTE_USER);
    }

    UInt64* pdpt = (UInt64*)PTE_GET_ADDR(pml4_virt[pml4_idx]);
    UInt64* pdpt_virt = sVMGetVirtualTable((UInt64)pdpt);

    if (!(pdpt_virt[pdpt_idx] & PTE_PRESENT)) {
        UInt64* addr = VMPhysicalMemoryAllocatePage();
        if (!addr) return nullptr;
        UInt64* addr_virt = sVMGetVirtualTable((UInt64)addr);
        memset(addr_virt, 0, kVMPageSize);
        pdpt_virt[pdpt_idx] = (UInt64)addr | table_flags;
    } else {
        pdpt_virt[pdpt_idx] |= (flags & PTE_USER);
    }

    UInt64* pd = (UInt64*)PTE_GET_ADDR(pdpt_virt[pdpt_idx]);
    UInt64* pd_virt = sVMGetVirtualTable((UInt64)pd);

    if (!(pd_virt[pd_idx] & PTE_PRESENT)) {
        UInt64* addr = VMPhysicalMemoryAllocatePage();
        if (!addr) return nullptr;
        UInt64* addr_virt = sVMGetVirtualTable((UInt64)addr);
        memset(addr_virt, 0, kVMPageSize);
        pd_virt[pd_idx] = (UInt64)addr | table_flags;
    } else {
        pd_virt[pd_idx] |= (flags & PTE_USER);
    }

    UInt64* pt = (UInt64*)PTE_GET_ADDR(pd_virt[pd_idx]);
    UInt64* pt_virt = sVMGetVirtualTable((UInt64)pt);
    pt_virt[pt_idx] = phys | flags;

    __asm__ volatile("invlpg (%0)" :: "r" (virt) : "memory");
    
    return (UInt64*)virt;
}

void VMVirtualMemoryUnmapPage(UInt64* pml4, UInt64 virt) {
    UInt64 pt_idx = VMM_PT_INDEX(virt);
    UInt64 pd_idx = VMM_PD_INDEX(virt);
    UInt64 pdpt_idx = VMM_PDPT_INDEX(virt);
    UInt64 pml4_idx = VMM_PML4_INDEX(virt);

    UInt64* pml4_virt = pml4;
    if (isInitialized) pml4_virt = (UInt64*)PHYS_TO_HHDM((UInt64)pml4);

    if (!(pml4_virt[pml4_idx] & PTE_PRESENT)) return;
    UInt64* pdpt_virt = sVMGetVirtualTable(PTE_GET_ADDR(pml4_virt[pml4_idx]));
    
    if (!(pdpt_virt[pdpt_idx] & PTE_PRESENT)) return;
    UInt64* pd_virt = sVMGetVirtualTable(PTE_GET_ADDR(pdpt_virt[pdpt_idx]));
    
    if (!(pd_virt[pd_idx] & PTE_PRESENT)) return;
    UInt64* pt_virt = sVMGetVirtualTable(PTE_GET_ADDR(pd_virt[pd_idx]));

    pt_virt[pt_idx] = 0; 
    
    __asm__ volatile("invlpg (%0)" :: "r" (virt) : "memory");
}

void VMLoadCR3(UInt64 pml4_addr) {
    __asm__ volatile ("mov %0, %%cr3" :: "r"(pml4_addr) : "memory");
}

void VMVirtualMemoryInitialize(Bootinfo* info) {
    gVMKernelPML4Physical = (UInt64)VMPhysicalMemoryAllocatePage();
    gVMKernelPML4 = (UInt64*)gVMKernelPML4Physical;
    memset(gVMKernelPML4, 0, kVMPageSize);

    UInt64 k_virt_start = (UInt64)&_kernel_start; 
    UInt64 k_virt_end   = (UInt64)&_kernel_end;

    UInt64 fb_start = (UInt64)info->framebuffer.base;
    UInt64 fb_end   = fb_start + info->framebuffer.baseSize;
    UInt64 fb_size = fb_end - fb_start;

    UInt64 max_mem = VMPhysicalMemoryGetTotalMemorySize();
    for (UInt64 i = 0; i < max_mem; i += kVMPageSize)               VMVirtualMemoryMapPage(gVMKernelPML4, i, PHYS_TO_HHDM(i), PTE_PRESENT | PTE_RW);
    for (UInt64 i = k_virt_start; i < k_virt_end; i += kVMPageSize) VMVirtualMemoryMapPage(gVMKernelPML4, KERNEL_VIRT_TO_PHYS(i), i, PTE_PRESENT | PTE_RW);
    for (UInt64 i = 0; i < fb_size; i += kVMPageSize)         VMVirtualMemoryMapPage(gVMKernelPML4, fb_start + i, FB_VIRT_BASE + i, PTE_PRESENT | PTE_RW);
    VMVirtualMemoryUnmapPage(gVMKernelPML4, (UInt64)&stack_guard);

    gVMPhycalMemoryBitmap = (UInt8*)PHYS_TO_HHDM((UInt64)gVMPhycalMemoryBitmap);
    info->framebuffer.base = (UInt32*)FB_VIRT_BASE;
    VMLoadCR3(gVMKernelPML4Physical);
    isInitialized = true;
}

UInt64 VMVirtualMemoryCreateAddressSpace() {
    UInt64 phys = (UInt64)VMPhysicalMemoryAllocatePage();
    if (!phys) return 0;

    UInt64* virt = (UInt64*)PHYS_TO_HHDM(phys);
    memset(virt, 0, kVMPageSize);

    UInt64* kernel_pml4_virt = sVMGetVirtualTable((UInt64)gVMKernelPML4);

    for (UInt32 i = 256; i < 512; i++) {
        virt[i] = kernel_pml4_virt[i];
    }

    return phys;
}

UInt64 VMGetCurrentCR3() {
    UInt64 cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}


void VMVirtualMemorySetupUserStack(UInt64* pml4_phys) {
    UInt64 stack_bottom = kVMUserStackTop - kVMUserStackSize;
    for (UInt64 addr = stack_bottom; addr < kVMUserStackTop; addr += 4096) {
        void* phys = VMPhysicalMemoryAllocatePage();
        if (!phys) OSPanic("OOM in user stack setup");
        memset((void*)PHYS_TO_HHDM((UInt64)phys), 0, 4096);
        VMVirtualMemoryMapPage((UInt64*)pml4_phys, (UInt64)phys, addr, PTE_PRESENT | PTE_RW | PTE_USER);
    }
}
