ENTRY(_start)

KERNEL_VMA_OFFSET = 0xFFFFFFFF80000000;
KERNEL_PHYS_START = 0x00100000;

SECTIONS
{
    . = KERNEL_VMA_OFFSET + KERNEL_PHYS_START;

    .text ALIGN(4096) : AT(ADDR(.text) - KERNEL_VMA_OFFSET) {
        *(.text.entry) 
        *(.text*)
    }

    .rodata ALIGN(4096) : AT(ADDR(.rodata) - KERNEL_VMA_OFFSET) {
        *(.rodata*)
    }

    .data ALIGN(4096) : AT(ADDR(.data) - KERNEL_VMA_OFFSET) {
        *(.data*)
    }

    .bss ALIGN(4096) : AT(ADDR(.bss) - KERNEL_VMA_OFFSET) {
        *(.bss*)
        *(COMMON)
    }

    .stack ALIGN(4096) : AT(ADDR(.stack) - KERNEL_VMA_OFFSET) {
        *(.stack)
    }

    _kernel_end = .;
}