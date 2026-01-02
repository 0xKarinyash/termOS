// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash
// i hate it
#![no_std]
#![no_main]

extern crate alloc;

use alloc::boxed::Box;
use alloc::vec;
use core::mem;
use log::info;
use uefi::boot::{self, AllocateType, MemoryType, MemoryDescriptor};
use uefi::prelude::*;
use uefi::proto::console::gop::GraphicsOutput;
use uefi::proto::media::file::{File, FileAttribute, FileInfo, FileMode};
use uefi::mem::memory_map::MemoryMap;

use elf::ElfBytes;
use elf::endian::AnyEndian;
use elf::abi::PT_LOAD;

#[global_allocator]
static ALLOCATOR: uefi::allocator::Allocator = uefi::allocator::Allocator;

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
#[derive(Debug)]
pub struct BootInfo {
    pub framebuffer: Framebuffer,
    pub map: *mut u8,
    pub map_size: usize,
    pub descriptor_size: usize,
    pub map_key: usize,
    pub phys_mem_offset: u64,
}

#[entry]
fn main() -> Status {
    uefi::helpers::init().unwrap();
    info!("Bootloader: Loading Kernel...");

    let gop_handle = boot::get_handle_for_protocol::<GraphicsOutput>().unwrap();
    let mut gop = boot::open_protocol_exclusive::<GraphicsOutput>(gop_handle).unwrap();
    let mode = gop.current_mode_info();
    let mut fb = gop.frame_buffer();
    let fb_struct = Framebuffer {
        base: fb.as_mut_ptr(),
        size: fb.size(),
        width: mode.resolution().0 as u32,
        height: mode.resolution().1 as u32,
        pitch: mode.stride() as u32,
    };
    drop(gop);

    let mut fs = boot::get_image_file_system(boot::image_handle()).unwrap();
    let mut root = fs.open_volume().unwrap();
    let mut file = root.open(cstr16!("kernel.elf"), FileMode::Read, FileAttribute::empty())
        .expect("Kernel ELF not found").into_regular_file().unwrap();

    let info_size = file.get_info::<FileInfo>(&mut []).unwrap_err().data().unwrap();
    let mut info_buf = vec![0u8; info_size];
    let file_info = file.get_info::<FileInfo>(&mut info_buf).unwrap();
    let mut elf_buf = vec![0u8; file_info.file_size() as usize];
    file.read(&mut elf_buf).unwrap();

    let elf = ElfBytes::<AnyEndian>::minimal_parse(&elf_buf).expect("Invalid ELF");
    let segments = elf.segments().expect("No segments");

    let kernel_vma_offset = 0xFFFFFFFF80000000u64;
    let mut kernel_loaded = false;

    for phdr in segments {
        if phdr.p_type == PT_LOAD {
             if phdr.p_vaddr < kernel_vma_offset {
                info!("Skipping segment V={:#x} (headers/garbage)", phdr.p_vaddr);
                continue;
            }

            kernel_loaded = true;

// Грузим физически начиная с 1 Мб, чтобы не затереть BIOS
            let phys_base = 0x100000; 
            let phys_addr = (phdr.p_vaddr - kernel_vma_offset) + phys_base;

            let mem_size = phdr.p_memsz as usize;
            let file_size = phdr.p_filesz as usize;
            let pages = (mem_size + 0xFFF) / 0x1000;

            info!("Seg: V={:#x} -> P={:#x} Pages={}", phdr.p_vaddr, phys_addr, pages);

            boot::allocate_pages(
                AllocateType::Address(phys_addr),
                MemoryType::LOADER_CODE,
                pages
            ).expect("Failed to allocate kernel pages (Busy?)");

            let dest = phys_addr as *mut u8;
            
            unsafe { core::ptr::write_bytes(dest, 0, mem_size) };
            
            if file_size > 0 {
                let data = elf.segment_data(&phdr).unwrap();
                unsafe { core::ptr::copy_nonoverlapping(data.as_ptr(), dest, file_size) };
            }
        }
    }

    if !kernel_loaded {
        panic!("Fatal: no kernel found!");
    }

    let boot_info = Box::leak(Box::new(BootInfo {
        framebuffer: fb_struct,
        map: core::ptr::null_mut(),
        map_size: 0,
        descriptor_size: 0,
        map_key: 0,
        phys_mem_offset: kernel_vma_offset,
    }));

    let memory_map = unsafe { boot::exit_boot_services(Some(MemoryType::LOADER_CODE)) };
    
    let mut entries = memory_map.entries();
    let first = entries.next().expect("Map empty");
    let first_addr = first as *const _ as usize;
    
    let stride = if let Some(second) = entries.next() {
        (second as *const _ as usize) - first_addr
    } else {
        core::mem::size_of::<MemoryDescriptor>()
    };
    

    let total_count = memory_map.entries().len();

    boot_info.map = first_addr as *mut u8;
    boot_info.descriptor_size = stride;
    boot_info.map_size = total_count * stride;

    core::mem::forget(memory_map);

    let virt_entry = elf.ehdr.e_entry;
    let phys_entry = virt_entry - kernel_vma_offset;

    let kmain: unsafe extern "sysv64" fn(*mut BootInfo) -> ! = unsafe { 
        mem::transmute(phys_entry as usize) 
    };

    unsafe {
        kmain(boot_info);
    }
}

// i really fucking hate this
