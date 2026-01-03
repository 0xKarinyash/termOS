#![no_std]
#![no_main]

mod boot;
pub mod drivers;
pub mod arch;
#[macro_use] mod macros;
mod panic;

use crate::boot::BootInfo;

#[unsafe(no_mangle)]
pub extern "C" fn kmain(boot_info: *const BootInfo) -> ! {
    arch::init();
    println!("Hi from termOS in rust!! With blackjack and bitches!");
    println!("Check this out: boot_info located at {:p}", boot_info);
    loop {}
}