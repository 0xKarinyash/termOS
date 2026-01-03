// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Framebuffer {
    pub base: *mut u8,
    pub size: usize,
    pub height: u32,
    pub width: u32,
    pub pitch: u32,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct MMap {
    pub map: *mut u8,
    pub map_size: usize,
    pub descriptor_size: usize,
    pub map_key: usize,
    pub phys_mem_offset: u64,
}

#[repr(C)]
#[derive(Debug)]
pub struct BootInfo {
    pub framebuffer: Framebuffer,
    pub mmap: MMap
}

pub const EFI_LOADER_CODE: u32 = 1;
pub const EFI_LOADER_DATA: u32 = 2;
pub const EFI_BOOT_SERVICES_CODE: u32 = 3;
pub const EFI_BOOT_SERVICES_DATA: u32 = 4;
pub const EFI_CONVENTIONAL_MEMORY: u32 = 7;
pub const EFI_ACPI_RECLAIM_MEMORY: u32 = 9;

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct EfiMemoryDescriptor {
    pub type_: u32,
    pub pad: u32, 
    pub phys_start: u64,
    pub virt_start: u64,
    pub number_of_pages: u64,
    pub attribute: u64,
}

pub struct MMapIter {
    curr: *mut u8,
    end: *mut u8,
    stride: usize,
}

impl MMap {
    pub fn iter(&self) -> MMapIter {
        MMapIter { curr: self.map, end: unsafe { self.map.add(self.map_size) }, stride: self.descriptor_size }
    }
}

impl Iterator for MMapIter {
    type Item = &'static EfiMemoryDescriptor;
    
    fn next(&mut self) -> Option<Self::Item> {
        if self.curr >= self.end { return None; }
        let desc = unsafe { &*(self.curr as * const EfiMemoryDescriptor ) }; // scary
        self.curr = unsafe { self.curr.add(self.stride) };
        Some(desc)
    }
}

