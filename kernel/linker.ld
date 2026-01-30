/* SPDX-License-Identifier: GPL-3.0-or-later
   Copyright (c) 2026 0xKarinyash */

ENTRY(_start)

KERNEL_VMA_OFFSET = 0xFFFFFFFF80000000;
KERNEL_PHYS_START = 0x00100000;

SECTIONS
{
    . = KERNEL_VMA_OFFSET + KERNEL_PHYS_START;

    _kernel_start = .;
    _kernel_phys_start = . - KERNEL_VMA_OFFSET;

    .text : AT(ADDR(.text) - KERNEL_VMA_OFFSET) {
        *(.text.entry) 
        *(.text*)
    }

    .rodata : AT(ADDR(.rodata) - KERNEL_VMA_OFFSET) {
        *(.rodata*)
    }

    .data : AT(ADDR(.data) - KERNEL_VMA_OFFSET) {
        *(.data*)
    }

    .bss : AT(ADDR(.bss) - KERNEL_VMA_OFFSET) {
        *(.bss*)
        *(COMMON)
    }

    .stack (ALIGN(16)) : AT(ADDR(.stack) - KERNEL_VMA_OFFSET) {
        *(.stack)
    }

    . = ALIGN(4096);
    
    _kernel_end = .;
    _kernel_phys_end = . - KERNEL_VMA_OFFSET;

    /DISCARD/ : {
        *(.note*)
        *(.comment*)
        *(.eh_frame*)
        *(.interp)  
        *(.dynsym)
        *(.dynstr)
    }
}