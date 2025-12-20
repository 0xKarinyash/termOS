ENTRY(kmain)

SECTIONS
{
    /* Смещаем точку сборки на 1МБ (как ты и грузишь) */
    . = 0x100000;

    /* Сначала идет КОД. Это самое важное. */
    .text : {
        *(.text.entry) /* Если будет ассемблерный трамплин */
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

    /* ВЫРЕЗАЕМ ВЕСЬ МУСОР LINUX/GNU */
    /DISCARD/ : {
        *(.note*)
        *(.comment*)
        *(.eh_frame*)
        *(.interp)     /* <--- Вот эта гадость содержала путь к ld-linux */
        *(.dynsym)
        *(.dynstr)
    }
}