#include "panic.h"
#include "console.h"
#include "rand.h"
#include "types.h"
#include "shitgui.h"

const char* fun_messages[] = {
    "Ooops! All red!",
    "Execution finished abnormally with code: KYS",
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
    "Fatal error has been occurred. Your device will be terminated in 30 seconds.",
    "I'll be back",
    "Hastla la vista, baby",
    "Ti chego mne tut nagovoril...",
    "Your access has been denied because of your region. Please, use Chultem VPN and try again later.",
    "Fatal error has been occurred. Your device will be transformed into Niva in a few seconds.",
    "Have you tried turning it off and on again?",
    "Put it in rice maybe?",
    "Just hit the monitor, it usually helps",
    "I think it needs a rest. Come back tomorrow",
    "You clicked something wrong, didn't you?",
    "You hacked it too hard, bro",
    "I think you deleted the internet",
    "Stop downloading RAM",
    "Pro tip: dont crash"
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

__attribute__((noreturn)) void panic(Registers *regs) {
    console_clear(0xFF0000);
    console_set_color(0xFFFFFF);

    SG_Point p = console_get_dimensions();
    p.x /= 2;
    p.y /= 2;
    p.y -= 200;
    
    console_set_cursor_pos(&p);

    u64 msg_count = sizeof(fun_messages) / sizeof(fun_messages[0]);
    u8 rand_num = shitrand() % msg_count;

    kprintf("\n\n");
    kprintf("\t\t\t\tKERNEL PANIC  :( \n");
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t\t\t%s\n", fun_messages[rand_num]);
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\tCPU EXCEPTION: %s (%d)\n", exception_messages[regs->int_no], regs->int_no);
    if (regs->err_code) kprintf("\t\tError Code: %x\n", regs->err_code);
    kprintf("\t\tInstruction Pointer (RIP): %x\n", regs->rip);
    kprintf("\t\tCode Segment (CS): %x\n", regs->cs);
    kprintf("\t\tFlags (RFLAGS): %x\n", regs->rflags);
    kprintf("\t\tStack Pointer (RSP): %x\n", regs->rsp);
    if (regs->int_no == 14) {
        u64 cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        kprintf("\t\tFaulting Address (CR2): %x\n", cr2);
    }
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t\t\tREGSISTERS\n");
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\tRAX=%x, RBX=%x, RCX=%x, RDX=%x\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
    kprintf("\t\tRSI=%x, RDI=%x, RBP=%x, R8=%x\n", regs->rsi, regs->rdi, regs->rbp, regs->r8);
    kprintf("\t\tR9=%x, R10=%x, R11=%x, R12=%x\n", regs->r9, regs->r10, regs->r11, regs->r12);
    kprintf("\t\tR13=%x, R14=%x, R15=%x\n", regs->r13, regs->r14, regs->r15);
    kprintf("\t\t--------------------------------\n");
    kprintf("\t\t\t\tSystem halted.\n");

    while (1) { 
        __asm__ volatile ("cli; hlt"); 
    }
}   