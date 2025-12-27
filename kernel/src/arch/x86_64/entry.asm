; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright (c) 2025 0xKarinyash

bits 64

section .bss
align 4096
guard_page:
    resb 4096

section .stack nobits
align 16
stack_bottom:
    resb 16384 
stack_top:

section .text
global _start
global gdt_flush
extern kmain

_start:
    cli

    mov rsp, stack_top
    xor rbp, rbp

    call kmain

.hang:
    hlt
    jmp .hang


gdt_flush:
    lgdt [rdi] 

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    pop rdi 
    push 0x08
    push rdi 
    retfq