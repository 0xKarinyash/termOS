// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <IO.h>
#include <OS/OSPanic.h>
#include <OS/OSScheduler.h>
#include <IO/IOKeyboard.h>
#include <IO/IOConsole.h>
#include <IO/IOTimer.h>
#include <OS/Services/OSServiceProcess.h>
#include <types.h>

void isr_handler_c(CPURegisters* frame) {
    if (frame->interruptNumber == 3) {
        IOConsoleLog("--------------------------------\n");
        IOConsoleLog("\t\t^gREGISTERS^!\n");
        IOConsoleLog("--------------------------------\n");
        IOConsoleLog("^gRAX^!=%X, ^gRBX^!=%X\n", frame->rax, frame->rbx);
        IOConsoleLog("^gRCX^!=%X, ^gRDX^!=%X\n", frame->rcx, frame->rdx);
        IOConsoleLog("^gRSI^!=%X, ^gRDI^!=%X\n", frame->rsi, frame->rdi);
        IOConsoleLog("^gRBP^!=%X, ^gR8^! =%X\n", frame->rbp, frame->r8);
        IOConsoleLog("^gR9^! =%X, ^gR10^!=%X \n", frame->r9, frame->r10);
        IOConsoleLog("^gR11^!=%X, ^gR12^!=%X\n", frame->r11, frame->r12);
        IOConsoleLog("^gR13^!=%X, ^gR14^!=%X\n", frame->r13, frame->r14);
        IOConsoleLog("^gR15^!=%X\n",frame->r15);
        IOConsoleLog("--------------------------------\n");
        return;
    }
    if ((frame->cs & 3) != 0) {
        IOConsoleLog("\n[Dewar] Process '%s' (PID %d) Segmentation Fault at %X\n", 
                gOSSchedulerCurrentTask->process->name, gOSSchedulerCurrentTask->process->processId, frame->rip);
        gOSSchedulerCurrentTask->taskState = kOSProcessStateDead;
        OSServiceProcessExit(-1);
        OSSchedulerNext((UInt64)frame);
        return;
    }
    OSPanicException(frame);
}

UInt64 irq_handler_c(CPURegisters* regs) {
    UInt64 curr_rsp = (UInt64)regs;
    switch (regs->interruptNumber) {
        case 32: curr_rsp = IOTimerInterruptsHandler(regs); break;
        case 33: IOKeyboardInterruptsHandler(); break;
        default: break;
    }

    IOPortWrite8(kIOMasterCommand, 0x20);
    return curr_rsp;
}