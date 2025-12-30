// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

__attribute__((noreturn)) void panic_exception(Registers *regs);
__attribute__((noreturn)) void panic(const char* msg);

extern const char* exception_messages[];
