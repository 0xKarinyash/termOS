#![no_std]
#![no_main]

mod boot;
mod io;
pub mod drivers;
#[macro_use] mod macros;
mod panic;
mod arch;

use crate::boot::BootInfo;

#[unsafe(no_mangle)]
pub extern "C" fn kmain(boot_info: *const BootInfo) -> ! {
    drivers::serial::COM1.lock().init();
    arch::gdt::init();
    arch::idt::init();
    println!("Hi from termOS in rust!! With blackjack and bitches!");
    println!("Check this out: boot_info located at {:p}", boot_info);
    loop {}
}