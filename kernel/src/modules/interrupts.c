#include "panic.h"
#include "types.h"

void isr_handler_c(Registers *regs) {
    panic(regs);
}