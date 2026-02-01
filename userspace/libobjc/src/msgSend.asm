; SPDX-License-Identifier: GPL-3.0-or-later
; Copyright (c) 2026 0xKarinyash

[bits 64]
section .text
global objc_msgSend

; id objc_msgSend(id self, SEL _cmd, ...)
; RDI = self, RSI = _cmd (pointer on OBJCSelector)

objc_msgSend:
    test rdi, rdi
    jz .return_nil
    mov rax, [rdi]

.search_class:
    ; OBJClass:
    ; 0: metaClass, 8: parentClass, 16: name, 24: version, 32: info, 40: instanceSize, 48: methods
    mov r10, [rax + 48] ; R10 = OBJCMethodList*

.search_method_list:
    test r10, r10
    jz .go_to_parent
    ; OBJCMethodList: 
    ; 0: next, 8: methodCount, 12: padding, 16: methods[]
    mov ecx, [r10 + 8] ; ECX = methodCount
    test ecx, ecx
    jz .next_list

    lea r11, [r10 + 16] ; R11 start of methods[]

.loop_methods:
    ; OBJCMethod:
    ; 0: selector, 8: types, 16: functionPointer
    mov r8, [r11]
    cmp r8, rsi
    je .found
    add r11, 24
    loop .loop_methods

.next_list:
    mov r10, [r10] ; r10 = list->next
    jmp .search_method_list

.go_to_parent:
    mov rax, [rax + 8]
    test rax, rax
    jnz .search_class
 
    jmp .return_nil

.found:
    mov rax, [r11 + 16]
    jmp rax

.return_nil:
    xor rax, rax
    xor rdx, rdx
    ret
