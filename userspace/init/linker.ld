ENTRY(start)

SECTIONS
{
    . = 0x400000;

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }

    /DISCARD/ : {
        *(.note.gnu.build-id)
        *(.note.GNU-stack)
        *(.note.gnu.property)
        *(.comment)
        *(.interp)
        *(.dynsym)
        *(.dynstr)
        *(.hash)
        *(.gnu.hash)
    }
}