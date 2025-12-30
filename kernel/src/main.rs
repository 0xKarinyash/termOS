#![no_std]
#![no_main]

use core::panic::PanicInfo;

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

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Framebuffer {
    pub base: *mut u8,
    pub size: usize,
    pub height: u32,
    pub width: u32,
    pub pitch: u32,
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

#[unsafe(no_mangle)]
pub extern "C" fn kmain(boot_info: usize) -> ! {
    loop {}
}