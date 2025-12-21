#include "panic.h"
#include "console.h"
#include "types.h"


const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

__attribute__((noreturn)) void panic(Registers *regs) {
    console_clear(0xFF0000);
    console_set_color(0xFFFFFF);

    kprintf("\n\n");
    kprintf("\t\t   KERNEL PANIC  :( \n");
    kprintf("\t\t-------------------\n");
    kprintf("\t\tCPU EXCEPTION: %d (%s)\n", regs->int_no, exception_messages[regs->int_no]);
    kprintf("\t\tError Code: 0x%x\n", regs->err_code);
    kprintf("\t\tInstruction Pointer (RIP): 0x%x\n", regs->rip);
    kprintf("\t\tCode Segment (CS): 0x%x\n", regs->cs);
    kprintf("\t\tFlags (RFLAGS): 0x%x\n", regs->rflags);
    kprintf("\t\tStack Pointer (RSP): 0x%x\n", regs->rsp);
    kprintf("\t\t-------------------\n");
    kprintf("   \tSystem halted.\n");

    while (1) { 
        __asm__ volatile ("cli; hlt"); 
    }
}   