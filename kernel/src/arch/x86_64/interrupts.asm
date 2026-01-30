; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright (c) 2025 0xKarinyash

bits 64

extern isr_handler_c
extern irq_handler_c

section .text

%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push 0
    push %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push %1
    jmp isr_common_stub
%endmacro

%macro PUSHALL 0
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
%endmacro

%macro POPALL 0
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
%endmacro

%macro IRQ_HANDLER 2
global %2
%2:
    push 0   ; dummy err code 
    push %1  ; int_no

    ; [rsp] = int_no (8)
    ; [rsp + 8] = err_code (8)
    ; [rsp + 16] = RIP (8)
    ; [rsp + 24] = CS (8) 

    test qword [rsp + 24], 3 
    jz .skip_swap
    swapgs     
    
.skip_swap:

    PUSHALL
    
    mov rdi, rsp
    cld 
    call irq_handler_c

    mov rsp, rax 

    ; PUSHALL - 15 * 8 = 120 bytes
    ; + int_no (8) + err_code (8) = 136 bytes
    ; [rsp + 136] = RIP
    ; [rsp + 144] = CS 
    
    test qword [rsp + 144], 3 
    jz .skip_swap_back
    swapgs
.skip_swap_back:

    POPALL
    add rsp, 16 
    iretq
%endmacro

ISR_NOERRCODE 0   ; Divide by zero
ISR_NOERRCODE 1   ; Debug
ISR_NOERRCODE 2   ; NMI
ISR_NOERRCODE 3   ; Breakpoint
ISR_NOERRCODE 4   ; Overflow
ISR_NOERRCODE 5   ; Bound Range Exceeded
ISR_NOERRCODE 6   ; Invalid Opcode
ISR_NOERRCODE 7   ; Device Not Available
ISR_ERRCODE   8   ; Double Fault
ISR_NOERRCODE 9   ; Coprocessor Segment Overrun
ISR_ERRCODE   10  ; Invalid TSS
ISR_ERRCODE   11  ; Segment Not Present
ISR_ERRCODE   12  ; Stack-Segment Fault
ISR_ERRCODE   13  ; General Protection Fault
ISR_ERRCODE   14  ; Page Fault
ISR_NOERRCODE 15  ; Reserved
ISR_NOERRCODE 16  ; x87 Floating-Point Exception
ISR_ERRCODE   17  ; Alignment Check
ISR_NOERRCODE 18  ; Machine Check
ISR_NOERRCODE 19  ; SIMD Floating-Point Exception
ISR_NOERRCODE 20  ; Virtualization Exception
ISR_NOERRCODE 21  ; Reserved
ISR_NOERRCODE 22  ; Reserved
ISR_NOERRCODE 23  ; Reserved
ISR_NOERRCODE 24  ; Reserved
ISR_NOERRCODE 25  ; Reserved
ISR_NOERRCODE 26  ; Reserved
ISR_NOERRCODE 27  ; Reserved
ISR_NOERRCODE 28  ; Reserved
ISR_NOERRCODE 29  ; Reserved
ISR_ERRCODE   30  ; Security Exception
ISR_NOERRCODE 31  ; Reserved

isr_common_stub:
    test qword [rsp + 24], 3
    jz .skip_swap
    swapgs
.skip_swap:
    PUSHALL
    mov rdi, rsp
    call isr_handler_c
    test qword [rsp + 144], 3
    jz .skip_swap_back
    swapgs
.skip_swap_back:
    POPALL
    add rsp, 16
    iretq

global idt_load
idt_load:
    lidt [rdi]
    ret

IRQ_HANDLER 32, irq0
IRQ_HANDLER 33, irq1