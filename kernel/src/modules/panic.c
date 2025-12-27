// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "panic.h"
#include "console.h"
#include "rand.h"
#include "types.h"
#include "shitgui.h"

const char* fun_messages[] = {
    "Execution finished abnormally with code: 0x_x",
    "It's definitely your fault.",
    "No more Roblox!",
    "Call your mom 4 help!",
    "2bad4u",
    "Touch grass",
    "Skill issue",
    "You should just go outside actually",
    "Perfect opportunity to take a shower",
    "404 not found",
    "Windows is locked! Password:___ Time left: 5:45:41",
    "GOOOOOAL",
    "\"NAM PIZDA\": hackers dropped our registry",
    "geeked vs locked in",
    "HEEEELP!",
    "LET ME GO!",
    "You little asshole, make it better",
    "Go and read books for programming u idiot",
    "TIME TO BRAKE! :3 Take a tea and try again later cutie",
    "Code have been eaten by Aliens",
    "That's all, folks!",
    "Raiden, answer me, Raiden, respond! Raiden?! RAIDEEEEEEEEEN!",
    "Fatal error has been occurred.\n\t\t\t\t Your device will be terminated in 30 seconds.",
    "I'll be back",
    "Hastla la vista, baby",
    "Ti chego mne tut nagovoril...",
    "Your access has been denied because of your region.\n\t\t\t\tPlease, use Chultem VPN and try again later.",
    "Fatal error has been occurred. \n\t\t\t\tYour device will be transformed into Niva in a few seconds.",
    "Have you tried turning it off and on again?",
    "Put it in rice maybe?",
    "Just hit the monitor, it usually helps",
    "I think it needs a rest. Come back tomorrow",
    "You clicked something wrong, didn't you?",
    "You hacked it too hard, bro",
    "I think you deleted the internet",
    "Stop downloading RAM",
    "Pro tip: dont crash",
    "Stop ricing the kernel you loonixtard, TermOS is NOT Loonix",
    "Attaboy, Jack. He's good. Really good.",
    "Zhenih priehal",
    "YOUR PC WAS BLOCKED BY FBI FOR WATCHING PORNOGRAPHY SEND 20 US DOLLARS",
    "fuck off"
};


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

__attribute__((noreturn)) void die() {
    while (1) { 
        __asm__ volatile ("cli; hlt"); 
    }
}

void draw_panic_bg() {
    console_clear(0x101010);
    console_set_color(0xFFFFFF);

    SG_Point p = console_get_dimensions();
    p.x /= 2;
    p.y /= 2;
    p.y -= 200;
    
    console_set_cursor_pos(&p);

    u64 msg_count = sizeof(fun_messages) / sizeof(fun_messages[0]);
    u8 rand_num = shitrand() % msg_count;

    kprintf("\n\n");
    kprintf("\t\t\t\t^tKERNEL PANIC^w  :( \n");
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t\t\t^r%s^w\n", fun_messages[rand_num]);
    kprintf("\t\t--------------------------------\n");
}

__attribute__((noreturn)) void panic_exception(Registers *regs) {
    draw_panic_bg();

    kprintf("\t\t^yCPU EXCEPTION^w: ^m%s^w (%d)\n", exception_messages[regs->int_no], regs->int_no);
    if (regs->err_code) kprintf("\t\t^yError Code^w: %X\n", regs->err_code);
    kprintf("\t\t^yInstruction Pointer^w (^yRIP^w): %X\n", regs->rip);
    kprintf("\t\t^yCode Segment^w (^yCS^w): %X\n", regs->cs);
    kprintf("\t\t^yFlags^w (^yRFLAGS^w): %X\n", regs->rflags);
    kprintf("\t\t^yStack Pointer^w (^yRSP^w): %X\n", regs->rsp);
    if (regs->int_no == 14) {
        u64 cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        kprintf("\t\t^yFaulting Address^w (^yCR2^w): %X\n", cr2);
    }
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t\t\t^yREGSISTERS^w\n");
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t^yRAX^w=%X, ^yRBX^w=%X\n", regs->rax, regs->rbx);
    kprintf("\t\t^yRCX^w=%X, ^yRDX^w=%X\n", regs->rcx, regs->rdx);
    kprintf("\t\t^yRSI^w=%X, ^yRDI^w=%X\n", regs->rsi, regs->rdi);
    kprintf("\t\t^yRBP^w=%X, ^yR8^w =%X\n", regs->rbp, regs->r8);
    kprintf("\t\t^yR9^w =%X, ^yR10^w=%X \n", regs->r9, regs->r10);
    kprintf("\t\t^yR11^w=%X, ^yR12^w=%X\n", regs->r11, regs->r12);
    kprintf("\t\t^yR13^w=%X, ^yR14^w=%X\n", regs->r13, regs->r14);
    kprintf("\t\t^yR15^w=%X\n",regs->r15);
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t\t\t^tSystem halted.^w\n");
    
    die();
} 

__attribute__((noreturn)) void panic(const char* msg) {
    draw_panic_bg();

    kprintf("\t\t^yReason^w: %s\n", msg);
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t\t\t^tSystem halted.^w\n");

    die();
}