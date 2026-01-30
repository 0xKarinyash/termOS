; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright (c) 2026 0xKarinyash

[bits 64]

section .text
global _start
extern main
extern sys_exit

_start:
    call main
    mov rdi, rax
    call sys_exit