// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <IDT.h>
#include <types.h>

HALInterruptsDescriptorTableEntry gHALInterruptsDescriptorTable[256];
HALInterruptsDescriptorTablePointer gHALInterruptsDescriptorTablePointer;

extern void isr0(); extern void isr1(); extern void isr2(); extern void isr3();
extern void isr4(); extern void isr5(); extern void isr6(); extern void isr7();
extern void isr8(); extern void isr9(); extern void isr10(); extern void isr11();
extern void isr12(); extern void isr13(); extern void isr14(); extern void isr15();
extern void isr16(); extern void isr17(); extern void isr18(); extern void isr19();
extern void isr20(); extern void isr21(); extern void isr22(); extern void isr23();
extern void isr24(); extern void isr25(); extern void isr26(); extern void isr27();
extern void isr28(); extern void isr29(); extern void isr30(); extern void isr31();
extern void irq0(void); extern void irq1(void);

extern void idt_load(UInt64); // asm: lidt [rdi] / ret

static void sHALInterruptsDescriptorTableSetGate(int number, UInt64 handler, UInt16 selector, UInt8 flags) {
    gHALInterruptsDescriptorTable[number].offsetLow  = handler & 0xFFFF;
    gHALInterruptsDescriptorTable[number].offsetMid  = (handler >> 16) & 0xFFFF;
    gHALInterruptsDescriptorTable[number].offsetHigh = (handler >> 32) & 0xFFFFFFFF;
    
    gHALInterruptsDescriptorTable[number].selector = selector; // 0x08 (Kernel Code)
    gHALInterruptsDescriptorTable[number].ist = 0;
    gHALInterruptsDescriptorTable[number].attributes = flags;
    gHALInterruptsDescriptorTable[number].reserved = 0;
}

void HALInterruptsDescriptorTableInitialize() {
    gHALInterruptsDescriptorTablePointer.limit = (sizeof(HALInterruptsDescriptorTableEntry) * 256) - 1;
    gHALInterruptsDescriptorTablePointer.base = (UInt64)&gHALInterruptsDescriptorTable;

    const UInt8 flags = 0x8E;
    const UInt8 selector = 0x08;
    

    sHALInterruptsDescriptorTableSetGate(0,  (UInt64)isr0,  selector, flags);
    sHALInterruptsDescriptorTableSetGate(1,  (UInt64)isr1,  selector, flags);
    sHALInterruptsDescriptorTableSetGate(2,  (UInt64)isr2,  selector, flags);
    sHALInterruptsDescriptorTableSetGate(3,  (UInt64)isr3,  selector, flags);
    sHALInterruptsDescriptorTableSetGate(4,  (UInt64)isr4,  selector, flags);
    sHALInterruptsDescriptorTableSetGate(5,  (UInt64)isr5,  selector, flags);
    sHALInterruptsDescriptorTableSetGate(6,  (UInt64)isr6,  selector, flags);
    sHALInterruptsDescriptorTableSetGate(7,  (UInt64)isr7,  selector, flags);
    sHALInterruptsDescriptorTableSetGate(8,  (UInt64)isr8,  selector, flags);
    sHALInterruptsDescriptorTableSetGate(9,  (UInt64)isr9,  selector, flags);
    sHALInterruptsDescriptorTableSetGate(10, (UInt64)isr10, selector, flags);
    sHALInterruptsDescriptorTableSetGate(11, (UInt64)isr11, selector, flags);
    sHALInterruptsDescriptorTableSetGate(12, (UInt64)isr12, selector, flags);
    sHALInterruptsDescriptorTableSetGate(13, (UInt64)isr13, selector, flags); 
    sHALInterruptsDescriptorTableSetGate(14, (UInt64)isr14, selector, flags); 
    sHALInterruptsDescriptorTableSetGate(15, (UInt64)isr15, selector, flags);
    sHALInterruptsDescriptorTableSetGate(16, (UInt64)isr16, selector, flags);
    sHALInterruptsDescriptorTableSetGate(17, (UInt64)isr17, selector, flags);
    sHALInterruptsDescriptorTableSetGate(18, (UInt64)isr18, selector, flags);
    sHALInterruptsDescriptorTableSetGate(19, (UInt64)isr19, selector, flags);
    sHALInterruptsDescriptorTableSetGate(20, (UInt64)isr20, selector, flags);
    sHALInterruptsDescriptorTableSetGate(21, (UInt64)isr21, selector, flags);
    sHALInterruptsDescriptorTableSetGate(22, (UInt64)isr22, selector, flags);
    sHALInterruptsDescriptorTableSetGate(23, (UInt64)isr23, selector, flags);
    sHALInterruptsDescriptorTableSetGate(24, (UInt64)isr24, selector, flags);
    sHALInterruptsDescriptorTableSetGate(25, (UInt64)isr25, selector, flags);
    sHALInterruptsDescriptorTableSetGate(26, (UInt64)isr26, selector, flags);
    sHALInterruptsDescriptorTableSetGate(27, (UInt64)isr27, selector, flags);
    sHALInterruptsDescriptorTableSetGate(28, (UInt64)isr28, selector, flags);
    sHALInterruptsDescriptorTableSetGate(29, (UInt64)isr29, selector, flags);
    sHALInterruptsDescriptorTableSetGate(30, (UInt64)isr30, selector, flags);
    sHALInterruptsDescriptorTableSetGate(31, (UInt64)isr31, selector, flags);

    gHALInterruptsDescriptorTable[8].ist = 1; // TSS
    gHALInterruptsDescriptorTable[13].ist = 1;
    gHALInterruptsDescriptorTable[14].ist = 1;

    sHALInterruptsDescriptorTableSetGate(32, (UInt64)irq0, selector, flags);
    sHALInterruptsDescriptorTableSetGate(33, (UInt64)irq1, selector, flags);

    __asm__ volatile ("lidt %0" : : "m"(gHALInterruptsDescriptorTablePointer));
}
