// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

use core::arch::asm;

#[inline(always)]
pub fn outb(port: u16, val: u8) {
    unsafe { 
        asm!(
            "out dx, al", 
            in("dx") port,
            in("al") val,
            options(nomem, nostack, preserves_flags)
        );
    }
}

#[inline(always)]
pub fn inb(port: u16) -> u8 {
    let res: u8;
    unsafe { 
        asm!(
            "in al, dx",
            out("al") res, 
            in("dx") port,
            options(nomem, nostack, preserves_flags)
        );
    }
    res
}