#pragma once
#ifndef PANIC_H
#define PANIC_H
#include "types.h"

__attribute__((noreturn)) void panic_exception(Registers *regs);
__attribute__((noreturn)) void panic(const char* msg);

extern const char* exception_messages[];

#endif