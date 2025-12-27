// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "idt.h"
#include "types.h"

IDTEntry idt[256];
IDTPtr idt_ptr;

extern void isr0(); extern void isr1(); extern void isr2(); extern void isr3();
extern void isr4(); extern void isr5(); extern void isr6(); extern void isr7();
extern void isr8(); extern void isr9(); extern void isr10(); extern void isr11();
extern void isr12(); extern void isr13(); extern void isr14(); extern void isr15();
extern void isr16(); extern void isr17(); extern void isr18(); extern void isr19();
extern void isr20(); extern void isr21(); extern void isr22(); extern void isr23();
extern void isr24(); extern void isr25(); extern void isr26(); extern void isr27();
extern void isr28(); extern void isr29(); extern void isr30(); extern void isr31();

extern void idt_load(u64); // asm: lidt [rdi] / ret

void idt_set_gate(int num, u64 handler, u16 sel, u8 flags) {
    idt[num].offset_low  = handler & 0xFFFF;
    idt[num].offset_mid  = (handler >> 16) & 0xFFFF;
    idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
    
    idt[num].selector = sel; // 0x08 (Kernel Code)
    idt[num].ist = 0;
    idt[num].attributes = flags;
    idt[num].reserved = 0;
}

void idt_init() {
    idt_ptr.limit = (sizeof(IDTEntry) * 256) - 1;
    idt_ptr.base = (u64)&idt;

    const u8 flags = 0x8E;
    const u8 selector = 0x08;
    

    idt_set_gate(0,  (u64)isr0,  selector, flags);
    idt_set_gate(1,  (u64)isr1,  selector, flags);
    idt_set_gate(2,  (u64)isr2,  selector, flags);
    idt_set_gate(3,  (u64)isr3,  selector, flags);
    idt_set_gate(4,  (u64)isr4,  selector, flags);
    idt_set_gate(5,  (u64)isr5,  selector, flags);
    idt_set_gate(6,  (u64)isr6,  selector, flags);
    idt_set_gate(7,  (u64)isr7,  selector, flags);
    idt_set_gate(8,  (u64)isr8,  selector, flags);
    idt_set_gate(9,  (u64)isr9,  selector, flags);
    idt_set_gate(10, (u64)isr10, selector, flags);
    idt_set_gate(11, (u64)isr11, selector, flags);
    idt_set_gate(12, (u64)isr12, selector, flags);
    idt_set_gate(13, (u64)isr13, selector, flags); 
    idt_set_gate(14, (u64)isr14, selector, flags); 
    idt_set_gate(15, (u64)isr15, selector, flags);
    idt_set_gate(16, (u64)isr16, selector, flags);
    idt_set_gate(17, (u64)isr17, selector, flags);
    idt_set_gate(18, (u64)isr18, selector, flags);
    idt_set_gate(19, (u64)isr19, selector, flags);
    idt_set_gate(20, (u64)isr20, selector, flags);
    idt_set_gate(21, (u64)isr21, selector, flags);
    idt_set_gate(22, (u64)isr22, selector, flags);
    idt_set_gate(23, (u64)isr23, selector, flags);
    idt_set_gate(24, (u64)isr24, selector, flags);
    idt_set_gate(25, (u64)isr25, selector, flags);
    idt_set_gate(26, (u64)isr26, selector, flags);
    idt_set_gate(27, (u64)isr27, selector, flags);
    idt_set_gate(28, (u64)isr28, selector, flags);
    idt_set_gate(29, (u64)isr29, selector, flags);
    idt_set_gate(30, (u64)isr30, selector, flags);
    idt_set_gate(31, (u64)isr31, selector, flags);

    idt[8].ist = 1; // TSS
    idt[13].ist = 1;
    idt[14].ist = 1;

    __asm__ volatile ("lidt %0" : : "m"(idt_ptr));
}
