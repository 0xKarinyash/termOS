// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

use crate::mm::bitmap::Bitmap;
use crate::boot::{*};
use spin::Mutex;

const PAGE_SIZE: u64 = 4096;
const SAFE_SPACE_START: u64 = 0x100000;

static PMM_BITMAP: Mutex<Option<Bitmap<'static>>> = Mutex::new(None);

unsafe extern "C" {
    static _kernel_phys_start: u8;
    static _kernel_phys_end: u8;
}

fn is_usable(t: u32) -> bool {
    matches!(t, 
        EFI_CONVENTIONAL_MEMORY |
        EFI_LOADER_CODE |
        EFI_LOADER_DATA |              
        EFI_BOOT_SERVICES_CODE |
        EFI_BOOT_SERVICES_DATA |
        EFI_ACPI_RECLAIM_MEMORY
    )
}

pub fn init(mmap: &MMap) {
    let mut pmm_bitmap = PMM_BITMAP.lock();

    let max_phys_addr = mmap.iter()
        .filter(|d| is_usable(d.type_)) // I looove rust
        .map(|d| d.phys_start + d.number_of_pages * PAGE_SIZE)
        .max().unwrap_or(0);

    let pages_count = max_phys_addr / PAGE_SIZE;
    let bitmap_size = ((pages_count + 7) / 8) as usize;

    let target_desc = mmap.iter()
        .find(|d| {
            d.type_ == EFI_CONVENTIONAL_MEMORY &&
            d.phys_start >= SAFE_SPACE_START &&
            d.number_of_pages * PAGE_SIZE >= bitmap_size as u64
        }).expect("Not enough RAM for PMM bitmap!!!");

    let bitmap_ptr = target_desc.phys_start as *mut u8;
    unsafe { core::ptr::write_bytes(bitmap_ptr, 0xFF, bitmap_size); }

    let mut bitmap = Bitmap::new(bitmap_ptr, bitmap_size);
    for desc in mmap.iter() {
        if desc.type_ == EFI_CONVENTIONAL_MEMORY {
            bitmap.free_reg(desc.phys_start, desc.number_of_pages * PAGE_SIZE);
        }
    }

    bitmap.alloc_reg(0, SAFE_SPACE_START);
    let k_start: u64;
    let k_end: u64;

    unsafe { 
        k_start = &_kernel_phys_start as *const _ as u64;
        k_end   = &_kernel_phys_end   as *const _ as u64;
    }

    bitmap.alloc_reg(k_start, k_end - k_start);
    bitmap.alloc_reg(target_desc.phys_start, bitmap_size as u64);
    *pmm_bitmap = Some(bitmap);
}