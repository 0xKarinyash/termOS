// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#![no_std]
#![no_main]

mod boot;
pub mod drivers;
pub mod arch;
#[macro_use] mod macros;
mod system;
mod mm;

use crate::boot::BootInfo;

#[unsafe(no_mangle)]
pub extern "C" fn kmain(boot_info: *const BootInfo) -> ! {
    println!("Kernel loaded!");
    arch::init();
    unsafe { mm::pmm::init(&(*boot_info).mmap); }
    println!("PMM initialized");
    println!("Hi from termOS in rust!! With blackjack and bitches!");
    println!("Check this out: boot_info located at {:p}", boot_info);
    loop {}
}