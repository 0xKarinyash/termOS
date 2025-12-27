// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "../common/bootinfo.h"

#include "vmm.h"
#include "gdt.h"
#include "idt.h"
#include "pmm.h"
#include "memory.h" // IWYU pragma: keep // shut the fuck up I DONT NEED <string.h> CLANGD PLEASE
#include "types.h"

u64* pml4_kernel = nullptr;

extern u64 _kernel_start; 
extern u64 _kernel_end;
extern u8* bitmap;
extern u64 bitmap_size_g;
extern GDTDescriptor gdt[];
extern TSS tss[];
extern IDTEntry idt[];
extern u8 double_fault_stack[];

void vmm_map_page(u64* pml4, u64 phys, u64 virt, u64 flags) {
    u64 pt_idx = VMM_PT_INDEX(virt);
    u64 pd_idx = VMM_PD_INDEX(virt);
    u64 pdpt_idx = VMM_PDPT_INDEX(virt);
    u64 pml4_idx = VMM_PML4_INDEX(virt);

    if (!(pml4[pml4_idx] & PTE_PRESENT)) {
        u64* addr = pmm_alloc_page();
        memset(addr, 0, PAGE_SIZE);
        pml4[pml4_idx] = (u64)addr | PTE_PRESENT | PTE_RW; 
    }

    u64* pdpt = (u64*)PTE_GET_ADDR(pml4[pml4_idx]);
    if (!(pdpt[pdpt_idx] & PTE_PRESENT)) {
        u64* addr = pmm_alloc_page();
        memset(addr, 0, PAGE_SIZE);
        pdpt[pdpt_idx] = (u64)addr | PTE_PRESENT | PTE_RW;
    }

    u64* pd = (u64*)PTE_GET_ADDR(pdpt[pdpt_idx]);
    if (!(pd[pd_idx] & PTE_PRESENT)) {
        u64* addr = pmm_alloc_page();
        memset(addr, 0, PAGE_SIZE);
        pd[pd_idx] = (u64)addr | PTE_PRESENT | PTE_RW;
    }

    u64* pt = (u64*)PTE_GET_ADDR(pd[pd_idx]);
    pt[pt_idx] = phys | flags;
}

static inline void load_cr3(u64 pml4_addr) {
    __asm__ volatile ("mov %0, %%cr3" :: "r"(pml4_addr) : "memory");
}

void vmm_init(Bootinfo* info) {
    pml4_kernel = pmm_alloc_page();
    memset(pml4_kernel, 0, PAGE_SIZE);

    u64 k_start = (u64)&_kernel_start; 
    u64 k_end   = (u64)&_kernel_end;

    u64 bitmap_start = (u64)bitmap;
    u64 bitmap_end   = bitmap_start + bitmap_size_g;

    u64 bi_addr = (u64)info;

    u64 fb_start = (u64)info->framebuffer.base;
    u64 fb_end   = fb_start + info->framebuffer.base_size;

    u64 gdt_addr = (u64)&gdt;
    u64 idt_addr = (u64)&idt;
    u64 tss_addr = (u64)&tss;
    u64 df_stack_addr = (u64)double_fault_stack;

    for (u64 i = k_start; i < k_end; i += PAGE_SIZE)           vmm_map_page(pml4_kernel, i, i, PTE_PRESENT | PTE_RW);
    for (u64 i = bitmap_start; i < bitmap_end; i += PAGE_SIZE) vmm_map_page(pml4_kernel, i, i, PTE_PRESENT | PTE_RW);
    for (u64 i = fb_start; i < fb_end; i += PAGE_SIZE)         vmm_map_page(pml4_kernel, i, i, PTE_PRESENT | PTE_RW);
    for (u64 i = 0; i < SAFE_SPACE_START_ADDR; i += PAGE_SIZE) vmm_map_page(pml4_kernel, i, i, PTE_PRESENT | PTE_RW);

    vmm_map_page(pml4_kernel, bi_addr, bi_addr,   PTE_PRESENT | PTE_RW);
    vmm_map_page(pml4_kernel, gdt_addr, gdt_addr, PTE_PRESENT | PTE_RW);
    vmm_map_page(pml4_kernel, idt_addr, idt_addr, PTE_PRESENT | PTE_RW);
    vmm_map_page(pml4_kernel, tss_addr, tss_addr, PTE_PRESENT | PTE_RW);
    
    vmm_map_page(pml4_kernel, df_stack_addr, df_stack_addr, PTE_PRESENT | PTE_RW);
    vmm_map_page(pml4_kernel, df_stack_addr + 4096, df_stack_addr + 4096, PTE_PRESENT | PTE_RW);

    load_cr3((u64)pml4_kernel);
}