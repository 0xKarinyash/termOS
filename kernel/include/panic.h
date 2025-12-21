#pragma once
#ifndef PANIC_H
#define PANIC_H
#include "types.h"

__attribute__((noreturn)) void panic(Registers *regs);

extern const char* exception_messages[];

#endif