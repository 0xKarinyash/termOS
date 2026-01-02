use core::mem::size_of;
use spin::Mutex;

#[repr(C, packed)]
#[derive(Copy, Clone)]
pub struct IdtEntry {
    offset_low: u16,
    selector: u16,
    ist: u8,
    attributes: u8,
    offset_mid: u16,
    offset_high: u32,
    reserved: u32,
}

#[repr(C, packed)]
pub struct IdtPtr {
    limit: u16,
    base: u64,
}

static IDT: Mutex<[IdtEntry; 256]> = Mutex::new([IdtEntry {
    offset_low: 0, selector: 0, ist: 0, attributes: 0, offset_mid: 0, offset_high: 0, reserved: 0
}; 256]);

unsafe extern "C" {
    fn isr0();  fn isr1();  fn isr2();  fn isr3();
    fn isr4();  fn isr5();  fn isr6();  fn isr7();
    fn isr8();  fn isr9();  fn isr10(); fn isr11();
    fn isr12(); fn isr13(); fn isr14(); fn isr15();
    fn isr16(); fn isr17(); fn isr18(); fn isr19();
    fn isr20(); fn isr21(); fn isr22(); fn isr23(); 
    fn isr24(); fn isr25(); fn isr26(); fn isr27(); 
    fn isr28(); fn isr29(); fn isr30(); fn isr31();
    fn irq0();  fn irq1();
}

pub fn init() {
    let mut idt = IDT.lock();

    let selector = 0x08; // Kernel Code
    let flags = 0x8E;    // Present, Ring0, Interrupt Gate

    let mut set_gate = |num: usize, handler: u64, sel: u16, flg: u8| {
        idt[num].offset_low = (handler & 0xFFFF) as u16;
        idt[num].offset_mid = ((handler >> 16) & 0xFFFF) as u16;
        idt[num].offset_high = ((handler >> 32) & 0xFFFFFFFF) as u32;
        idt[num].selector = sel;
        idt[num].ist = 0;
        idt[num].attributes = flg;
        idt[num].reserved = 0;
    };

    set_gate(0,  isr0  as *const () as u64, selector, flags);
    set_gate(1,  isr1  as *const () as u64, selector, flags);
    set_gate(2,  isr2  as *const () as u64, selector, flags);
    set_gate(3,  isr3  as *const () as u64, selector, flags);
    set_gate(4,  isr4  as *const () as u64, selector, flags);
    set_gate(5,  isr5  as *const () as u64, selector, flags);
    set_gate(6,  isr6  as *const () as u64, selector, flags);
    set_gate(7,  isr7  as *const () as u64, selector, flags);
    set_gate(8,  isr8  as *const () as u64, selector, flags);
    set_gate(9,  isr9  as *const () as u64, selector, flags);
    set_gate(10, isr10 as *const () as u64, selector, flags);
    set_gate(11, isr11 as *const () as u64, selector, flags);
    set_gate(12, isr12 as *const () as u64, selector, flags);
    set_gate(13, isr13 as *const () as u64, selector, flags);
    set_gate(14, isr14 as *const () as u64, selector, flags);
    set_gate(15, isr15 as *const () as u64, selector, flags);
    set_gate(16, isr16 as *const () as u64, selector, flags);
    set_gate(17, isr17 as *const () as u64, selector, flags);
    set_gate(18, isr18 as *const () as u64, selector, flags);
    set_gate(19, isr19 as *const () as u64, selector, flags);
    set_gate(20, isr20 as *const () as u64, selector, flags);
    set_gate(21, isr21 as *const () as u64, selector, flags);
    set_gate(22, isr22 as *const () as u64, selector, flags);
    set_gate(23, isr23 as *const () as u64, selector, flags);
    set_gate(24, isr24 as *const () as u64, selector, flags);
    set_gate(25, isr25 as *const () as u64, selector, flags);
    set_gate(26, isr26 as *const () as u64, selector, flags);
    set_gate(27, isr27 as *const () as u64, selector, flags);
    set_gate(28, isr28 as *const () as u64, selector, flags);
    set_gate(29, isr29 as *const () as u64, selector, flags);
    set_gate(30, isr30 as *const () as u64, selector, flags);
    set_gate(31, isr31 as *const () as u64, selector, flags);
    set_gate(32, irq0 as *const () as u64, selector, flags); 
    set_gate(33, irq1 as *const () as u64, selector, flags);
    
    idt[8].ist = 1; 
    idt[13].ist = 1;
    idt[14].ist = 1;

    unsafe {
    
        let idt_ptr = IdtPtr {
            limit: (size_of::<[IdtEntry; 256]>() - 1) as u16,
            base: idt.as_ptr() as u64,
        };

        core::arch::asm!("lidt [{}]", in(reg) &idt_ptr);
    }
}