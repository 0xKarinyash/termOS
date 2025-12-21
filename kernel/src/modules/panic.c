#include "panic.h"
#include "console.h"
#include "types.h"
#include "shitgui.h"


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

    SG_Point p = console_get_dimensions();
    p.x /= 2;
    p.y /= 2;
    p.y -= 200;
    
    console_set_cursor_pos(&p);

    kprintf("\n\n");
    kprintf("\t\t\tKERNEL PANIC  :( \n");
    kprintf("\t\t-------------------------\n");
    kprintf("\t\tCPU EXCEPTION: %d (%s)\n", regs->int_no, exception_messages[regs->int_no]);
    kprintf("\t\tError Code: %x\n", regs->err_code);
    kprintf("\t\tInstruction Pointer (RIP): %x\n", regs->rip);
    kprintf("\t\tCode Segment (CS): %x\n", regs->cs);
    kprintf("\t\tFlags (RFLAGS): %x\n", regs->rflags);
    kprintf("\t\tStack Pointer (RSP): %x\n", regs->rsp);
    kprintf("\t\t-------------------------\n");
    kprintf("\t\t\tSystem halted.\n");

    while (1) { 
        __asm__ volatile ("cli; hlt"); 
    }
}   