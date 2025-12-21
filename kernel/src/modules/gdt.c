// just fucking kill me already

#include "gdt.h"

GDTDescriptor gdt[3];
GDTPtr gdt_ptr;

void gdt_set_gate(int num, u64 base, u64 limit, u8 access, u8 gran) {
    gdt[num].base_low = (base & 0xFFFF); // left only low 16bit
    gdt[num].base_middle = (base >> 16) & 0xFF; // 3rd address byte
    gdt[num].base_high = (base >> 24) & 0xFF; // 4th address byte

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F; // leave only first 4 bits (limit's left 16-19)
    gdt[num].granularity |= gran & 0xF0; // get flags and concate two pieces so FFFFLLLL (L -- Limit; F -- flags)
    gdt[num].access = access;

    // intel,
    // please
    // FUCK YOU
    // cancer of industry
    // AMD FOREVER
}

extern void gdt_flush(u64 gdt_ptr_addr); // entry.asm

void gdt_init() {
    gdt_ptr.limit = (sizeof(GDTDescriptor) * 3) - 1;
    gdt_ptr.base = (u64)&gdt;

    // 0: Null
    gdt_set_gate(0, 0, 0, 0, 0);

    // 1: Kernel Code (Ring 0)
    // Access: 0x9A (Present, Ring0, Exec/Read)
    // Granularity: 0x20 (Long Mode bit)                    access  ring 0    code  exec    code    readable
    gdt_set_gate(1, 0, 0, 0x9A, 0x20);              //  0x9A = 1    00          1   1       0       1
                                                    //  0x20 = 0010 0000 bit 5 is "Long mode"
    // 2: Kernel Data (Ring 0)
    // Access: 0x92 (Present, Ring0, Read/Write)
    gdt_set_gate(2, 0, 0, 0x92, 0);                 // 0x92 can't exec

    gdt_flush((u64)&gdt_ptr);
}