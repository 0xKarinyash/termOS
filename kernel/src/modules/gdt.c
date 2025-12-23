// just fucking kill me already

#include "gdt.h"

GDTDescriptor gdt[5];
GDTPtr gdt_ptr;
TSS tss;
u8 double_fault_stack[4096] = {0};

void gdt_set_gate(int num, u64 base, u64 limit, u8 access, u8 gran) {
    gdt[num].base_low = (base & 0xFFFF); // left only low 16bit
    gdt[num].base_middle = (base >> 16) & 0xFF; // 3rd address byte
    gdt[num].base_high = (base >> 24) & 0xFF; // 4th address byte

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F; // leave only first 4 bits (limit's left 16-19)
    gdt[num].granularity |= gran & 0xF0; // get flags and concate two pieces so FFFFLLLL (L -- Limit; F -- flags)
    gdt[num].access = access;

    // fuck intel
}


void write_tss(int num) {
    u64 base = (u64)&tss;
    u64 limit = sizeof(tss);
    u64 access = 0x89; // TSS_PRESENT | TSS_EXECUTABLE | TSS_ACCESSED;
    u64 gran = 0;

    gdt_set_gate(num, base, limit, access, gran);
     *((u64*)&gdt[num+1]) = (base >> 32);   // really scary but not really hard
                                            // just direct write 8bytes to num+1
                                            // basically a hack but at this point i dont care
}


extern void gdt_flush(u64 gdt_ptr_addr); // entry.asm

void gdt_init() {
    gdt_ptr.limit = (sizeof(GDTDescriptor) * 5) - 1;
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

    for (u64 i = 0; i < sizeof(TSS); i++) ((u8*)&tss)[i] = 0; // hack. zeroifying tss struct

    tss.iomap_base = sizeof(TSS);
    tss.ist1 = (u64)double_fault_stack + sizeof(double_fault_stack);
    write_tss(3);

    gdt_flush((u64)&gdt_ptr);

    // telling cpu that TSS info located at gdt[3]; offset = 3 * 8 = 24 = 0x18
    __asm__ volatile ("ltr %%ax" :: "a" (0x18));
}

