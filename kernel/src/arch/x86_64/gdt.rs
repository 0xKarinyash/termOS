// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

use core::mem::size_of;
use core::ptr;
use spin::Mutex;

#[repr(C, packed)]
#[derive(Copy, Clone, Debug)]
pub struct GdtEntry {
    limit_low: u16,
    base_low: u16,
    base_middle: u8,
    access: u8,
    granularity: u8,
    base_high: u8,
}

#[repr(C, packed)]
pub struct GdtPtr {
    limit: u16,
    base: u64,
}

#[repr(C, packed)]
pub struct Tss {
    reserved0: u32,
    rsp0: u64,
    rsp1: u64,
    rsp2: u64,
    reserved1: u64,
    ist1: u64,
    ist2: u64,
    ist3: u64,
    ist4: u64,
    ist5: u64,
    ist6: u64,
    ist7: u64,
    reserved2: u64,
    reserved3: u16,
    iomap_base: u16,
}

static GDT: Mutex<[GdtEntry; 6]> = Mutex::new([GdtEntry { limit_low: 0, base_low: 0, base_middle: 0, access: 0, granularity: 0, base_high: 0 }; 6]);
static TSS_ENTRY: Mutex<Tss> = Mutex::new(Tss {
    reserved0: 0, rsp0: 0, rsp1: 0, rsp2: 0, reserved1: 0, 
    ist1: 0, ist2: 0, ist3: 0, ist4: 0, ist5: 0, ist6: 0, ist7: 0, 
    reserved2: 0, reserved3: 0, iomap_base: 0 
});
static DOUBLE_FAULT_STACK: Mutex<[u8; 4096]> = Mutex::new([0; 4096]);


pub fn init() {
    let mut gdt = GDT.lock();
    let mut tss = TSS_ENTRY.lock();
    let dfs = DOUBLE_FAULT_STACK.lock();

    let mut set_gate = |num: usize, base: u64, limit: u64, access: u8, gran: u8| {
        gdt[num].base_low = (base & 0xFFFF) as u16;
        gdt[num].base_middle = ((base >> 16) & 0xFF) as u8;
        gdt[num].base_high = ((base >> 24) & 0xFF) as u8;
        gdt[num].limit_low = (limit & 0xFFFF) as u16;
        gdt[num].granularity = (((limit >> 16) & 0x0F) as u8) | (gran & 0xF0);
        gdt[num].access = access;
    };

    // 0: Null
    set_gate(0, 0, 0, 0, 0);

    // 1: Kernel Code (Ring 0) -> 0x9A
    // Granularity 0x20 = Long Mode
    set_gate(1, 0, 0, 0x9A, 0x20);

    // 2: Kernel Data (Ring 0) -> 0x92
    set_gate(2, 0, 0, 0x92, 0);

    tss.iomap_base = size_of::<Tss>() as u16;
    tss.ist1 = (dfs.as_ptr() as u64) + size_of::<[u8; 4096]>() as u64;

    let tss_base = &TSS_ENTRY as *const _ as u64;
    let tss_limit = size_of::<Tss>() as u64;

    // 3: TSS (Low part)
    // Access: 0x89 (Present, Executable, Accessed)
    set_gate(3, tss_base, tss_limit, 0x89, 0);

    // hack for tss high part (64bit descriptors are 16bytes)
    // C's *((u64*)&gdt[num+1])...
    let high_part_ptr = &mut gdt[4] as *mut _ as *mut u64;

    unsafe {
        ptr::write(high_part_ptr, tss_base >> 32);

        let gdt_ptr = GdtPtr {
            limit: (size_of::<[GdtEntry; 6]>() - 1) as u16,
            base: gdt.as_ptr() as u64,
        };
        // load_gdt
        core::arch::asm!(
            "lgdt [{0}]",       // Load GDT
            "push 0x08",        // Push Code Segment (offset 0x08)
            "lea rax, [rip + 2f]", // Load address of label 2 into RAX
            "push rax",         // Push return address
            "retfq",            // Far return (pops CS and RIP) - reloads CS
            "2:",
            "mov ax, 0x10",     // Data Segment (offset 0x10)
            "mov ds, ax",
            "mov es, ax",
            "mov fs, ax",
            "mov gs, ax",
            "mov ss, ax",
            in(reg) &gdt_ptr,
            out("rax") _, 
        );
        // load_tss
        core::arch::asm!("ltr ax", in("ax") 0x18u16);
    }
}
