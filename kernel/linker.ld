ENTRY(_start)

SECTIONS
{
    . = 0x100000;

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

    /DISCARD/ : {
        *(.note*)
        *(.comment*)
        *(.eh_frame*)
        *(.interp)  
        *(.dynsym)
        *(.dynstr)
    }
}