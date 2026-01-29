// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/memory.h>

#include <gdt.h>
#include <idt.h>

#include <types.h>
#include "bootinfo.h"

u64* pml4_kernel = nullptr;
u64 pml4_kernel_phys = 0;
static bool is_initialized = false;

extern u64 _kernel_start; 
extern u64 _kernel_end;
extern u8* bitmap;
extern u64 bitmap_size_g;
extern GDTDescriptor gdt[];
extern IDTEntry idt[];
extern u8 double_fault_stack[];
extern u8 stack_guard;

static u64* get_table_virt(u64 phys) {
    if (is_initialized) return (u64*)PHYS_TO_HHDM(phys);
    return (u64*)phys;
}

u64* vmm_map_page(u64* pml4, u64 phys, u64 virt, u64 flags) {
    u64 pt_idx = VMM_PT_INDEX(virt);
    u64 pd_idx = VMM_PD_INDEX(virt);
    u64 pdpt_idx = VMM_PDPT_INDEX(virt);
    u64 pml4_idx = VMM_PML4_INDEX(virt);

    u64* pml4_virt = pml4;
    if (is_initialized) pml4_virt = (u64*)PHYS_TO_HHDM((u64)pml4);

    u64 table_flags = PTE_PRESENT | PTE_RW | (flags & PTE_USER);

    if (!(pml4_virt[pml4_idx] & PTE_PRESENT)) {
        u64* addr = pmm_alloc_page();
        if (!addr) return nullptr;
        u64* addr_virt = get_table_virt((u64)addr);
        memset(addr_virt, 0, PAGE_SIZE);
        pml4_virt[pml4_idx] = (u64)addr | table_flags; 
    } else {
        pml4_virt[pml4_idx] |= (flags & PTE_USER);
    }

    u64* pdpt = (u64*)PTE_GET_ADDR(pml4_virt[pml4_idx]);
    u64* pdpt_virt = get_table_virt((u64)pdpt);

    if (!(pdpt_virt[pdpt_idx] & PTE_PRESENT)) {
        u64* addr = pmm_alloc_page();
        if (!addr) return nullptr;
        u64* addr_virt = get_table_virt((u64)addr);
        memset(addr_virt, 0, PAGE_SIZE);
        pdpt_virt[pdpt_idx] = (u64)addr | table_flags;
    } else {
        pdpt_virt[pdpt_idx] |= (flags & PTE_USER);
    }

    u64* pd = (u64*)PTE_GET_ADDR(pdpt_virt[pdpt_idx]);
    u64* pd_virt = get_table_virt((u64)pd);

    if (!(pd_virt[pd_idx] & PTE_PRESENT)) {
        u64* addr = pmm_alloc_page();
        if (!addr) return nullptr;
        u64* addr_virt = get_table_virt((u64)addr);
        memset(addr_virt, 0, PAGE_SIZE);
        pd_virt[pd_idx] = (u64)addr | table_flags;
    } else {
        pd_virt[pd_idx] |= (flags & PTE_USER);
    }

    u64* pt = (u64*)PTE_GET_ADDR(pd_virt[pd_idx]);
    u64* pt_virt = get_table_virt((u64)pt);
    pt_virt[pt_idx] = phys | flags;

    __asm__ volatile("invlpg (%0)" :: "r" (virt) : "memory");
    
    return (u64*)virt;
}

void vmm_unmap_page(u64* pml4, u64 virt) {
    u64 pt_idx = VMM_PT_INDEX(virt);
    u64 pd_idx = VMM_PD_INDEX(virt);
    u64 pdpt_idx = VMM_PDPT_INDEX(virt);
    u64 pml4_idx = VMM_PML4_INDEX(virt);

    u64* pml4_virt = pml4;
    if (is_initialized) pml4_virt = (u64*)PHYS_TO_HHDM((u64)pml4);

    if (!(pml4_virt[pml4_idx] & PTE_PRESENT)) return;
    u64* pdpt_virt = get_table_virt(PTE_GET_ADDR(pml4_virt[pml4_idx]));
    
    if (!(pdpt_virt[pdpt_idx] & PTE_PRESENT)) return;
    u64* pd_virt = get_table_virt(PTE_GET_ADDR(pdpt_virt[pdpt_idx]));
    
    if (!(pd_virt[pd_idx] & PTE_PRESENT)) return;
    u64* pt_virt = get_table_virt(PTE_GET_ADDR(pd_virt[pd_idx]));

    pt_virt[pt_idx] = 0; 
    
    __asm__ volatile("invlpg (%0)" :: "r" (virt) : "memory");
}

void load_cr3(u64 pml4_addr) {
    __asm__ volatile ("mov %0, %%cr3" :: "r"(pml4_addr) : "memory");
}

void vmm_init(Bootinfo* info) {
    pml4_kernel_phys = (u64)pmm_alloc_page();
    pml4_kernel = (u64*)pml4_kernel_phys;
    memset(pml4_kernel, 0, PAGE_SIZE);

    u64 k_virt_start = (u64)&_kernel_start; 
    u64 k_virt_end   = (u64)&_kernel_end;

    u64 fb_start = (u64)info->framebuffer.base;
    u64 fb_end   = fb_start + info->framebuffer.base_size;
    u64 fb_size = fb_end - fb_start;

    u64 max_mem = pmm_get_total_mem();
    for (u64 i = 0; i < max_mem; i += PAGE_SIZE)               vmm_map_page(pml4_kernel, i, PHYS_TO_HHDM(i), PTE_PRESENT | PTE_RW);
    for (u64 i = k_virt_start; i < k_virt_end; i += PAGE_SIZE) vmm_map_page(pml4_kernel, KERNEL_VIRT_TO_PHYS(i), i, PTE_PRESENT | PTE_RW);
    for (u64 i = 0; i < fb_size; i += PAGE_SIZE)         vmm_map_page(pml4_kernel, fb_start + i, FB_VIRT_BASE + i, PTE_PRESENT | PTE_RW);
    vmm_unmap_page(pml4_kernel, (u64)&stack_guard);

    bitmap = (u8*)PHYS_TO_HHDM((u64)bitmap);
    info->framebuffer.base = (u32*)FB_VIRT_BASE;
    load_cr3(pml4_kernel_phys);
    is_initialized = true;
}

u64 vmm_create_address_space() {
    u64 phys = (u64)pmm_alloc_page();
    if (!phys) return 0;

    u64* virt = (u64*)PHYS_TO_HHDM(phys);
    memset(virt, 0, PAGE_SIZE);

    u64* kernel_pml4_virt = get_table_virt((u64)pml4_kernel);

    for (u32 i = 256; i < 512; i++) {
        virt[i] = kernel_pml4_virt[i];
    }

    return phys;
}

u64 vmm_get_current_cr3() {
    u64 cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}