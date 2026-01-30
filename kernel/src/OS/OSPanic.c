// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <OS/OSPanic.h>
#include <lib/Rand.h>

#include <IO/IOGraphics.h>
#include <IO/IOConsole.h>

#include <types.h>

static const char* sOSPanicFunMessages[] = {
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


static const char* sOSPanicExceptionMessages[] = {
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

__attribute__((noreturn)) static void die() {
    while (1) { 
        __asm__ volatile ("cli; hlt"); 
    }
}

static void sOSPanicDrawPanicBackground() {
    IOConsoleClear(0x000000);
    IOConsoleSetForegroundColor(0xFFFFFF);
    IOConsoleSetDefaultForegroundColor(0xFFFFFF);
    
    UInt64 funMessagesCount = sizeof(sOSPanicFunMessages) / sizeof(sOSPanicFunMessages[0]);
    UInt8 randNum = Rand() % funMessagesCount;

    IOConsoleLog("\n\n");
    IOConsoleLog("\t\t\t\t^bKERNEL PANIC^!  :( \n");
    IOConsoleLog("\t\t--------------------------------\n");
    IOConsoleLog("\t\t^y%s^!\n", sOSPanicFunMessages[randNum]);
    IOConsoleLog("\t\t--------------------------------\n");
}

__attribute__((noreturn)) void OSPanicException(CPURegisters* frame) {
    sOSPanicDrawPanicBackground();

    IOConsoleLog("\t\t^yCPU EXCEPTION^!: ^b%s^! (%d)\n", sOSPanicExceptionMessages[frame->interruptNumber], frame->interruptNumber);
    if (frame->errorCode) IOConsoleLog("\t\t^yError Code^!: %X\n", frame->errorCode);
    IOConsoleLog("\t\t^yInstruction Pointer^! (^yRIP^!): %X\n", frame->rip);
    IOConsoleLog("\t\t^yCode Segment^! (^yCS^!): %X\n", frame->cs);
    IOConsoleLog("\t\t^yFlags^! (^yRFLAGS^!): %X\n", frame->rflags);
    IOConsoleLog("\t\t^yStack Pointer^! (^yRSP^!): %X\n", frame->rsp);
    if (frame->interruptNumber == 14) {
        IOConsoleLog("\t\t--------------------------------\n");
        IOConsoleLog("\t\t       ^yPage Fault Helper^!        \n");
        IOConsoleLog("\t\t--------------------------------\n");
        UInt64 cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        IOConsoleLog("\t\t^yFaulting Address^! (^yCR2^!): %X\n", cr2);
        IOConsoleLog("\t\t^yERRCode^!: %X\n", frame->errorCode);
        UInt64 present     = (frame->errorCode & (1 << 0)) != 0;
        UInt64 write       = (frame->errorCode & (1 << 1)) != 0;
        UInt64 user        = (frame->errorCode & (1 << 2)) != 0;
        UInt64 reserved    = (frame->errorCode & (1 << 3)) != 0;
        UInt64 instruction = (frame->errorCode & (1 << 4)) != 0;
        IOConsoleLog("\t\t\t[^bP^!] ^yReason^!: %s\n", present ? "Page Protection violation" : "Non-present page");
        IOConsoleLog("\t\t\t[^bW^!] ^yCaused by^! %s\n", write ? "WRITE" : "READ"); 
        IOConsoleLog("\t\t\t[^bU^!] ^yRing^! %s\n", user ? "3" : "0");
        if (reserved) IOConsoleLog("\t\t\t[^bR^!] CPU Wrote 1 to a reserved field in page table entry. ^rCorrupt page table?^!\n");
        if (instruction) IOConsoleLog("\t\t\t[^bI^!] ^yTried to^! execute ^ycode from^! NX ^ymemory^!\n");
    }
    IOConsoleLog("\t\t--------------------------------\n");
    IOConsoleLog("\t\t\t\t^yREGISTERS^!\n");
    IOConsoleLog("\t\t--------------------------------\n");
    IOConsoleLog("\t\t^yRAX^!=%X, ^yRBX^!=%X\n",  frame->rax, frame->rbx);
    IOConsoleLog("\t\t^yRCX^!=%X, ^yRDX^!=%X\n",  frame->rcx, frame->rdx);
    IOConsoleLog("\t\t^yRSI^!=%X, ^yRDI^!=%X\n",  frame->rsi, frame->rdi);
    IOConsoleLog("\t\t^yRBP^!=%X, ^yR8^! =%X\n",  frame->rbp, frame->r8);
    IOConsoleLog("\t\t^yR9^! =%X, ^yR10^!=%X \n", frame->r9,  frame->r10);
    IOConsoleLog("\t\t^yR11^!=%X, ^yR12^!=%X\n",  frame->r11, frame->r12);
    IOConsoleLog("\t\t^yR13^!=%X, ^yR14^!=%X\n",  frame->r13, frame->r14);
    IOConsoleLog("\t\t^yR15^!=%X\n",frame->r15);
    IOConsoleLog("\t\t--------------------------------\n");
    IOConsoleLog("\t\t\t\t^bSystem halted.^!\n");
    
    die();
} 

__attribute__((noreturn)) void OSPanic(const char* message) {
    sOSPanicDrawPanicBackground();

    IOConsoleLog("\t\t^yReason^!: %s\n", message);
    IOConsoleLog("\t\t--------------------------------\n");
    IOConsoleLog("\t\t\t\t^bSystem halted.^!\n");

    die();
}