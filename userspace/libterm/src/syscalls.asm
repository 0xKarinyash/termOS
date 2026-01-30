; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright (c) 2026 0xKarinyash

bits 64

section .text

global sys_exit
global sys_spawn
global sys_mem
global sys_write
global sys_read

sys_exit:
    mov rax, 0
    syscall
    ret

sys_spawn:
    mov rax, 1
    syscall
    ret

sys_mem:
    mov rax, 2
    syscall
    ret

sys_write:
    mov rax, 3
    syscall
    ret

sys_read:
    mov rax, 4
    syscall
    ret
