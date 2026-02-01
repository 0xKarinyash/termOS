; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright (c) 2026 0xKarinyash

[bits 64]

section .text
global _start
extern main
extern OSServiceProcessExit
extern _objc_init_runtime

_start:
    call _objc_init_runtime 

    call main
    
    mov rdi, rax
    call OSServiceProcessExit