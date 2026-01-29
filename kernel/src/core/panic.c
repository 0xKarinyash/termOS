// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <core/panic.h>
#include <core/rand.h>

#include <drivers/shitgui.h>
#include <drivers/console.h>

#include <types.h>

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
    "fuck off",
    "Your GPU is now mining Ethereum for me. Thanks.",
    "Your PC is locked!\n\t\t\t\tPay a fee of 0.019082006 bitcoins to a wallet 1KtoProchitalTotSosal to get decryption key!",
};


const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception", 
    "Reserved"          
};

__attribute__((noreturn)) void die() {
    while (1) { 
        __asm__ volatile ("cli; hlt"); 
    }
}

void draw_panic_bg() {
    console_clear(0x000000);
    console_set_color(0xFFFFFF);
    console_set_default_color(0xFFFFFF);
    
    u64 msg_count = sizeof(fun_messages) / sizeof(fun_messages[0]);
    u8 rand_num = krand() % msg_count;

    kprintf("\n\n");
    kprintf("\t\t\t\t^bKERNEL PANIC^!  :( \n");
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t^y%s^!\n", fun_messages[rand_num]);
    kprintf("\t\t--------------------------------\n");
}

__attribute__((noreturn)) void panic_exception(Registers *regs) {
    draw_panic_bg();

    kprintf("\t\t^yCPU EXCEPTION^!: ^b%s^! (%d)\n", exception_messages[regs->int_no], regs->int_no);
    if (regs->err_code) kprintf("\t\t^yError Code^!: %X\n", regs->err_code);
    kprintf("\t\t^yInstruction Pointer^! (^yRIP^!): %X\n", regs->rip);
    kprintf("\t\t^yCode Segment^! (^yCS^!): %X\n", regs->cs);
    kprintf("\t\t^yFlags^! (^yRFLAGS^!): %X\n", regs->rflags);
    kprintf("\t\t^yStack Pointer^! (^yRSP^!): %X\n", regs->rsp);
    if (regs->int_no == 14) {
        kprintf("\t\t--------------------------------\n");
        kprintf("\t\t       ^yPage Fault Helper^!        \n");
        kprintf("\t\t--------------------------------\n");
        u64 cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        kprintf("\t\t^yFaulting Address^! (^yCR2^!): %X\n", cr2);
        kprintf("\t\t^yERRCode^!: %X\n", regs->err_code);
        u64 present     = (regs->err_code & (1 << 0)) != 0;
        u64 write       = (regs->err_code & (1 << 1)) != 0;
        u64 user        = (regs->err_code & (1 << 2)) != 0;
        u64 reserved    = (regs->err_code & (1 << 3)) != 0;
        u64 instruction = (regs->err_code & (1 << 4)) != 0;
        kprintf("\t\t\t[^bP^!] ^yReason^!: %s\n", present ? "Page Protection violation" : "Non-present page");
        kprintf("\t\t\t[^bW^!] ^yCaused by^! %s\n", write ? "WRITE" : "READ"); 
        kprintf("\t\t\t[^bU^!] ^yRing^! %s\n", user ? "3" : "0");
        if (reserved) kprintf("\t\t\t[^bR^!] CPU Wrote 1 to a reserved field in page table entry. ^rCorrupt page table?^!\n");
        if (instruction) kprintf("\t\t\t[^bI^!] ^yTried to^! execute ^ycode from^! NX ^ymemory^!\n");
    }
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t\t\t^yREGISTERS^!\n");
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t^yRAX^!=%X, ^yRBX^!=%X\n", regs->rax, regs->rbx);
    kprintf("\t\t^yRCX^!=%X, ^yRDX^!=%X\n", regs->rcx, regs->rdx);
    kprintf("\t\t^yRSI^!=%X, ^yRDI^!=%X\n", regs->rsi, regs->rdi);
    kprintf("\t\t^yRBP^!=%X, ^yR8^! =%X\n", regs->rbp, regs->r8);
    kprintf("\t\t^yR9^! =%X, ^yR10^!=%X \n", regs->r9, regs->r10);
    kprintf("\t\t^yR11^!=%X, ^yR12^!=%X\n", regs->r11, regs->r12);
    kprintf("\t\t^yR13^!=%X, ^yR14^!=%X\n", regs->r13, regs->r14);
    kprintf("\t\t^yR15^!=%X\n",regs->r15);
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t\t\t^bSystem halted.^!\n");
    
    die();
} 

__attribute__((noreturn)) void panic(const char* msg) {
    draw_panic_bg();

    kprintf("\t\t^yReason^!: %s\n", msg);
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t\t\t^bSystem halted.^!\n");

    die();
}