// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <io.h>
#include <core/panic.h>
#include <core/scheduler.h>
#include <drivers/keyboard.h>
#include <drivers/console.h>
#include <drivers/timer.h>
#include <shell/builtins.h>
#include <syscalls/proc.h>
#include <types.h>

extern task* curr_task;

void isr_handler_c(Registers *regs) {
    if (regs->int_no == 3) {
        return print_regs();
    }
    if ((regs->cs & 3) != 0) {
        kprintf("\n[Dewar] Process '%s' (PID %d) Segmentation Fault at %X\n", 
                curr_task->proc->name, curr_task->proc->pid, regs->rip);
        curr_task->task_state = DEAD;
        sys_exit(-1);
        sched_next((u64)regs);
        return;
    }
    panic_exception(regs);
}

u64 irq_handler_c(Registers *regs) {
    u64 curr_rsp = (u64)regs;
    switch (regs->int_no) {
        case 32: curr_rsp = timer_handler(regs); break;
        case 33: kb_handler(); break;
        default: break;
    }

    outb(MASTER_COMMAND, 0x20);
    return curr_rsp;
}