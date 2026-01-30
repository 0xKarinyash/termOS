// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

__attribute__((noreturn)) void OSPanicException(CPURegisters* frame);
__attribute__((noreturn)) void OSPanic(const char* msg);
