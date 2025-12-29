// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#ifndef VMM_H
#define VMM_H

#include "bootinfo.h"
#include <types.h>

#define PTE_PRESENT  (1ULL << 0)    // 0 -- not present: page fault when trying to access; 1 -- present, can RW
#define PTE_RW       (1ULL << 1)    // 0 -- RO: page fault trying to write; 1 -- RW
#define PTE_USER     (1ULL << 2)    // 0 -- Ring 0: R3 get segfault gtrying to access; 1 -- Ring 3
#define PTE_PWT      (1ULL << 3)    // 1 -- skip L1/L2 cache when writing to RAM
#define PTE_PCD      (1ULL << 4)    // 1 -- cache disabled for page
#define PTE_ACCESSED (1ULL << 5)    // CPU sets it (by itself) to 1 when RW this page
#define PTE_DIRTY    (1ULL << 6)    // CPU sets it when writing to this page
#define PTE_NX       (1ULL << 63)   // No execute


#define PTE_ADDR_MASK 0x000FFFFFFFFFF000 // mask to get clean physical addres that located at bits 12-51

#define PTE_GET_ADDR(entry)  ((entry) & PTE_ADDR_MASK)  // get physical address
#define PTE_GET_FLAGS(entry) ((entry) & ~PTE_ADDR_MASK) // get flags

#define VMM_PT_INDEX(virt)   (((virt) >> 12) & 0x1FF) // Table Index (bits 12-20)
#define VMM_PD_INDEX(virt)   (((virt) >> 21) & 0x1FF) // Page Directory Index (bits 21-29) 
#define VMM_PDPT_INDEX(virt) (((virt) >> 30) & 0x1FF) // PDPT Index (bits 30-38)
#define VMM_PML4_INDEX(virt) (((virt) >> 39) & 0x1FF) // PML4 Index (bits 39-47): main page


#define KERNEL_VMA          0xFFFFFFFF80000000
#define HHDM_OFFSET         0xFFFF888000000000
#define FB_VIRT_BASE        0xFFFFFFFFFC000000

#define KERNEL_VIRT_TO_PHYS(virt) ((virt) - KERNEL_VMA)
#define PHYS_TO_HHDM(phys)  ((phys) + HHDM_OFFSET)
#define HHDM_TO_PHYS(virt)  ((virt) - HHDM_OFFSET)


void vmm_init(Bootinfo* info);
u64* vmm_map_page(u64* pml4, u64 phys, u64 virt, u64 flags);

#endif