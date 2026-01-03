; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright (c) 2025 0xKarinyash

bits 64

; Определяем константы
%define KERNEL_VMA      0xFFFFFFFF80000000
%define PAGE_PRESENT    (1 << 0)
%define PAGE_RW         (1 << 1)
%define PAGE_HUGE       (1 << 7) 

section .text.entry
global _start
extern kmain

_start:
    cli
    mov r12, rdi

    mov rsp, stack_top
    mov rax, KERNEL_VMA
    sub rsp, rax 
    
    mov rdi, pml4_table
    sub rdi, rax
    mov rsi, pdpt_table
    sub rsi, rax
    mov rdx, pd_table
    sub rdx, rax

    ; PML4
    mov rax, rsi         ; rax = phys(pdpt)
    or  rax, PAGE_PRESENT | PAGE_RW
    
    ; Identity mapping
    mov [rdi], rax       ; pml4[0] -> pdpt
    
    ; Higher half mapping
    mov [rdi + 511 * 8], rax ; pml4[511] -> pdpt

    ; PDPT
    mov rax, rdx         ; rax = phys(pd)
    or  rax, PAGE_PRESENT | PAGE_RW
    
    ; Identity mapping (0-1GB)
    mov [rsi], rax       ; pdpt[0] -> pd
    
    ; Kernel Space mapping (0xFFFFFFFF80...)
    ; 510 = -2GB
    mov [rsi + 510 * 8], rax ; pdpt[510] -> pd

    mov rcx, 0
    mov rbx, 512
.fill_pd_loop:
    mov r8, rcx
    or  r8, PAGE_PRESENT | PAGE_RW | PAGE_HUGE
    mov [rdx], r8
    add rcx, 0x200000
    add rdx, 8
    dec rbx
    jnz .fill_pd_loop

    mov rax, pml4_table
    mov rbx, KERNEL_VMA
    sub rax, rbx
    mov cr3, rax

    ; Enable SSE 
    mov rax, cr0
    and ax, 0xFFFB
    or ax, 0x2 
    mov cr0, rax
    mov rax, cr4
    or ax, 3 << 9
    mov cr4, rax

    mov rax, .higher_half
    jmp rax

.higher_half:
    mov rsp, stack_top
    mov rdi, r12
    xor rbp, rbp
    
    call kmain

.hang:
    hlt
    jmp .hang

section .bss
align 4096
pml4_table: resb 4096
pdpt_table: resb 4096
pd_table:   resb 4096

section .stack nobits
align 4096
stack_bottom: resb 16384
stack_top: