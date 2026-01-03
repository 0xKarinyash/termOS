use crate::panic::panic_exception;
use crate::println;


#[repr(C)]
#[derive(Debug)]
pub struct TrapFrame {
    // Pushed by PUSHALL
    pub r15: u64,
    pub r14: u64,
    pub r13: u64,
    pub r12: u64,
    pub r11: u64,
    pub r10: u64,
    pub r9: u64,
    pub r8: u64,
    pub rbp: u64,
    pub rdi: u64,
    pub rsi: u64,
    pub rdx: u64,
    pub rcx: u64,
    pub rbx: u64,
    pub rax: u64,
    
    // Pushed by ISR stub
    pub int_num: u64,
    pub error_code: u64,
    
    // Pushed by CPU
    pub rip: u64,
    pub cs: u64,
    pub rflags: u64,
    pub rsp: u64,
    pub ss: u64,
}

#[unsafe(no_mangle)]
pub extern "C" fn isr_handler_c(frame: &mut TrapFrame) {
    panic_exception(&frame);
}

#[unsafe(no_mangle)]
pub extern "C" fn irq_handler_c(frame: &mut TrapFrame) {
    println!("{:?}", frame);
}