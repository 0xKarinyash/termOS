; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright (c) 2025 0xKarinyash

bits 64

%define KERNEL_VMA      0xFFFFFFFF80000000
%define PAGE_PRESENT    (1 << 0)
%define PAGE_RW         (1 << 1)
%define PAGE_HUGE       (1 << 7) 

section .bss
align 4096
pml4_table:
    resb 4096
pdpt_table:
    resb 4096
pd_table:
    resb 4096

section .stack nobits
align 4096

global stack_guard
stack_guard:
    resb 4096

global stack_bottom
stack_bottom:
    resb 16384
stack_top:

section .text
global _start
global gdt_flush
extern kmain

_start:
    cli

    mov r12, rdi 

    mov rsp, stack_top
    mov rax, KERNEL_VMA
    sub rsp, rax
    
    mov rdi, pml4_table
    sub rdi, KERNEL_VMA

    mov rsi, pdpt_table
    sub rsi, KERNEL_VMA

    mov rdx, pd_table
    sub rdx, KERNEL_VMA

    mov rax, rsi
    or  rax, PAGE_PRESENT | PAGE_RW
    mov [rdi], rax

    mov [rdi + 4088], rax

    mov rax, rdx
    or  rax, PAGE_PRESENT | PAGE_RW
    mov [rsi], rax

    mov [rsi + 4080], rax

    mov rcx, 0
    mov rbx, 512
    mov rax, rdx

.fill_pd_loop:
    mov r8, rcx
    or  r8, PAGE_PRESENT | PAGE_RW | PAGE_HUGE
    mov [rax], r8

    add rcx, 0x200000
    add rax, 8
    dec rbx
    jnz .fill_pd_loop
    mov cr3, rdi

    mov rax, .higher_half
    jmp rax

.higher_half:
    mov rsp, stack_top 
    mov rdi, r12
    xor rbp, rbp

    call kmain

.hang:
    cli
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