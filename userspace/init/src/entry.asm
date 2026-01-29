[BITS 64]

start:
    mov rax, 2          ; SYS_WRITE
    mov rdi, msg_hello  ; Адрес строки
    syscall

    mov rax, 1          ; SYS_EXEC
    syscall

    mov rax, 0          ; SYS_EXIT
    mov rdi, 1337
    syscall

    jmp $

msg_hello: db "Hello from init!", 0xA, 0