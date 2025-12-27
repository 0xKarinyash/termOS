/* SPDX-License-Identifier: GPL-3.0-or-later
   Copyright (c) 2025 0xKarinyash */

ENTRY(_start)

SECTIONS
{
    . = 0x100000;
    _kernel_start = .;

    .text : {
        *(.text.entry) 
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
        *(COMMON)
    }

    .stack (ALIGN(16)) : {
        *(.stack)
    }

    . = ALIGN(4096);
    _kernel_end = .;

    /DISCARD/ : {
        *(.note*)
        *(.comment*)
        *(.eh_frame*)
        *(.interp)  
        *(.dynsym)
        *(.dynstr)
    }
}