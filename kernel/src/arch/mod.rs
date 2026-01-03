// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#[cfg(target_arch = "x86_64")]
pub mod x86_64;

#[cfg(target_arch = "x86_64")]
pub use x86_64::*;
use crate::{drivers, println};

pub fn init() {
    drivers::serial::COM1.lock().init();
    println!("Serial initialized");
    gdt::init();
    println!("GDT & TSS initialized");
    idt::init();
    println!("IDT initialized");
}