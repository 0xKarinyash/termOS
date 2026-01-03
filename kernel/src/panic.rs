use core::panic::PanicInfo;
use crate::arch::x86_64::interrupts::TrapFrame;

const EXCEPTION_MESSAGES: [&str; 32] = [
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
];


#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

fn die() -> ! {
    loop {
        unsafe {
            core::arch::asm!("cli; hlt");
        }
    }
}

pub fn panic_exception(frame: &TrapFrame) {
    println!("Kernel panic!!");
    let msg = EXCEPTION_MESSAGES.get(frame.int_num as usize).unwrap_or(&"Unknown exception");
    println!("CPU Exception: {msg} ({})", frame.int_num);
    println!("Instruction Poiter (RIP): 0x{:016X}", frame.rip);
    println!("Code Segment (CS): 0x{:016X}", frame.cs);
    println!("Flags (RFLAGS): 0x{:016X}", frame.rflags);
    println!("Stack Pointer: (RSP): 0x{:016X}", frame.rsp);
    if frame.int_num == 14 {
        let cr2: u64;
        unsafe {
            core::arch::asm!("mov {}, cr2", out(reg) cr2);
        }
        println!("Faulting Address (CR2): 0x{:016X}", cr2);
        println!("ERRCode: 0x{:b}", frame.error_code);
        let present     = (frame.error_code & (1 << 0)) != 0;
        let write       = (frame.error_code & (1 << 1)) != 0;
        let user        = (frame.error_code & (1 << 2)) != 0;
        let reserved    = (frame.error_code & (1 << 3)) != 0;
        let instruction = (frame.error_code & (1 << 4)) != 0;
        println!("  [P] {}", if present { "Page Protection violation"} else { "Non-present page" });
        println!("  [W] Caused by {}", if write { "WRITE" } else { "READ" });
        println!("  [U] Ring {}", if user { "3" } else { "0" });
        if reserved {
            println!("  [R] CPU Wrote 1 to a reserved field in page table entry (CORRUPT PAGE TABLE)");
        }
        if instruction {
            println!("  [I] Tried to execute code from NX memory");
        }
    }
    println!("Registers");
    println!("RAX = 0x{:016X}, RBX = 0x{:016X}", frame.rax, frame.rbx);
    println!("RCX = 0x{:016X}, RDX = 0x{:016X}", frame.rcx, frame.rdx);
    println!("RSI = 0x{:016X}, RDI = 0x{:016X}", frame.rsi, frame.rdi);
    println!("RBP = 0x{:016X}, R8  = 0x{:016X}", frame.rbp, frame.r8);
    println!("R9  = 0x{:016X}, R10 = 0x{:016X}", frame.r9,  frame.r10);
    println!("R11 = 0x{:016X}, R12 = 0x{:016X}", frame.r11, frame.r12);
    println!("R13 = 0x{:016X}, R14 = 0x{:016X}", frame.r13, frame.r14);
    println!("R15 = 0x{:016X}", frame.r15);
    println!("System halted");
    die();
}