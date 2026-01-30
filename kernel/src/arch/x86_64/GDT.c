// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

// just fucking kill me already

#include <GDT.h>

HALGlobalDescriptorTable gHALGlobalDescriptorTable[kHALGDTEntries];
HALGlobalDescriptorTablePointer gHALGlobalDescriptorTablePointer;
HALTaskStateSegment gHALTaskStateSegment;
UInt8 gHALDoubleFaultStack[4096] = {0};

static void sHALGlobalDescriptorTableSetGate(int number, UInt64 base, UInt64 limit, UInt8 access, UInt8 granularity) {
    gHALGlobalDescriptorTable[number].baseLow = (base & 0xFFFF); // left only low 16bit
    gHALGlobalDescriptorTable[number].baseMiddle = (base >> 16) & 0xFF; // 3rd address byte
    gHALGlobalDescriptorTable[number].baseHigh = (base >> 24) & 0xFF; // 4th address byte

    gHALGlobalDescriptorTable[number].limitLow = (limit & 0xFFFF);
    gHALGlobalDescriptorTable[number].granularity = (limit >> 16) & 0x0F; // leave only first 4 bits (limit's left 16-19)
    gHALGlobalDescriptorTable[number].granularity |= granularity & 0xF0; // get flags and concate two pieces so FFFFLLLL (L -- Limit; F -- flags)
    gHALGlobalDescriptorTable[number].access = access;

    // fuck intel
}


static void sHALTaskStateSegmentWrite(int number) {
    UInt64 base = (UInt64)&gHALTaskStateSegment;
    UInt64 limit = sizeof(gHALTaskStateSegment);
    UInt64 access = 0x89; // TSS_PRESENT | TSS_EXECUTABLE | TSS_ACCESSED;
    UInt64 granularity = 0;

    sHALGlobalDescriptorTableSetGate(number, base, limit, access, granularity);
     *((UInt64*)&gHALGlobalDescriptorTable[number+1]) = (base >> 32);   // really scary but not really hard
                                            // just direct write 8bytes to num+1
                                            // basically a hack but at this point i dont care
}


extern void gdt_flush(UInt64 gdt_ptr_addr); // entry.asm

void HALGlobalDescriptorTableInitialize() {
    gHALGlobalDescriptorTablePointer.limit = (sizeof(HALGlobalDescriptorTable) * kHALGDTEntries) - 1;
    gHALGlobalDescriptorTablePointer.base = (UInt64)&gHALGlobalDescriptorTable;

    // 0: Null
    sHALGlobalDescriptorTableSetGate(0, 0, 0, 0, 0);

    // 1: Kernel Code (Ring 0)
    // Access: 0x9A (Present, Ring0, Exec/Read)
    // Granularity: 0x20 (Long Mode bit)                    access  ring 0    code  exec    code    readable
    sHALGlobalDescriptorTableSetGate(1, 0, 0, 0x9A, 0x20);              //  0x9A = 1    00          1   1       0       1
                                                    //  0x20 = 0010 0000 bit 5 is "Long mode"
    // 2: Kernel Data (Ring 0)
    // Access: 0x92 (Present, Ring0, Read/Write)
    sHALGlobalDescriptorTableSetGate(2, 0, 0, 0x92, 0);                 //  0x92 = 1    00          1   0       0       1

    // 3: User Data (Ring 3)
    // Access: 0xF2 (Present, Ring3, Read/Write)
    sHALGlobalDescriptorTableSetGate(3, 0, 0, 0xF2, 0);                 //  0xF2 = 1    11          1   0       0       1

    // 4: User Code (Ring 3)
    // Access: 0xFA (Present, Ring3, RWX)
    sHALGlobalDescriptorTableSetGate(4, 0, 0, 0xFA, 0x20);              //  0xFA = 1    11          1   1       0       1

    for (UInt64 i = 0; i < sizeof(HALTaskStateSegment); i++) ((UInt8*)&gHALTaskStateSegment)[i] = 0; // zeroifying tss struct

    gHALTaskStateSegment.iomapBase = sizeof(HALTaskStateSegment);
    gHALTaskStateSegment.ist1 = (UInt64)gHALDoubleFaultStack + sizeof(gHALDoubleFaultStack);
    sHALTaskStateSegmentWrite(5);

    gdt_flush((UInt64)&gHALGlobalDescriptorTablePointer);

    // telling cpu that TSS info located at gdt[5]; offset = 5 * 8 = 40 = 0x28
    __asm__ volatile ("ltr %%ax" :: "a" (0x28));
}

