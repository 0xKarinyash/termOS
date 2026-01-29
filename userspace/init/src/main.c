static inline void sys_write(const char* str) {
    __asm__ volatile (
        "mov $2, %%rax\n\t"   
        "mov %0, %%rdi\n\t"
        "syscall"
        :
        : "r"(str)
        : "rax", "rdi", "rcx", "r11", "memory" 
    );
}

int main() {
    sys_write("Meowww!!!\n");
    sys_write("Nyaaaaa");
    for (int i = 0; i < 100; i++) sys_write("a"); 
}