// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <idt.h>
#include <types.h>

IDTEntry idt[256];
IDTPtr idt_ptr;

extern "C" {
    void isr0(); void isr1(); void isr2(); void isr3();
    void isr4(); void isr5(); void isr6(); void isr7();
    void isr8(); void isr9(); void isr10(); void isr11();
    void isr12(); void isr13(); void isr14(); void isr15();
    void isr16(); void isr17(); void isr18(); void isr19();
    void isr20(); void isr21(); void isr22(); void isr23();
    void isr24(); void isr25(); void isr26(); void isr27();
    void isr28(); void isr29(); void isr30(); void isr31();
    void irq0(void); void irq1(void);
}

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

    idt_set_gate(32, (u64)irq0, selector, flags);
    idt_set_gate(33, (u64)irq1, selector, flags);

    __asm__ volatile ("lidt %0" : : "m"(idt_ptr));
}
