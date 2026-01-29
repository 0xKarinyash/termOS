; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright (c) 2026 0xKarinyash

bits 64

extern syscall_dispatch
global syscall_entry

section .text

syscall_entry:
    swapgs
    mov [gs:0], rsp ; saving users rsp in g_cpu.user_rsp
    mov rsp, [gs:8] ; load kernel rsp from g_cpu.kernel_rsp

    push qword [gs:0]
    push r11 ; rflags
    push rcx ; rip

    push rbp
    push rbx
    push rdi
    push rsi
    push rdx
    push r10
    push r8
    push r9
    push r12
    push r13
    push r14
    push r15

    ; preparing c code
    ; c waits rdi, rsi, rdx, rcx, r8, r9
    ; user sent rax, rdi, rsi, rdx, r10, r8 (rax=id)

    mov r9,  r8             ; 6th arg
    mov r8,  r10            ; 5th arg
    mov rcx, rdx            ; 4th arg
    mov rdx, rsi            ; 3rd arg
    mov rsi, rdi            ; 2nd arg
    mov rdi, rax            ; 1st arg (id)

    call syscall_dispatch

    ; restoring context except RAX (result there)
    pop r15
    pop r14
    pop r13
    pop r12
    pop r9
    pop r8
    pop r10
    pop rdx
    pop rsi
    pop rdi
    pop rbx
    pop rbp
    pop rcx ; rip
    pop r11 ; rflags
    add rsp, 8 ; skip rsp

    mov [gs:8], rsp
    mov rsp, [gs:0]
    swapgs

    o64 sysret


    
