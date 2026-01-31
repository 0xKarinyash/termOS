; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright (c) 2026 0xKarinyash

bits 64

section .text

global OSServiceProcessExit
global OSServiceProcessSpawn
global OSServiceMemoryAllocate
global OSServiceIOWrite
global OSServiceIORead
global OSServiceProcessWait

OSServiceProcessExit:
    mov rax, 0
    syscall
    ret

OSServiceProcessSpawn:
    mov rax, 1
    syscall
    ret

OSServiceMemoryAllocate:
    mov rax, 2
    syscall
    ret

OSServiceIOWrite:
    mov rax, 3
    syscall
    ret

OSServiceIORead:
    mov rax, 4
    syscall
    ret

OSServiceProcessWait:
    mov rax, 5
    syscall
    ret