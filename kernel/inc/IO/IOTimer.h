// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

void IOTimerInitialize(UInt32 freq);
UInt64 IOTimerInterruptsHandler(CPURegisters *regs);
UInt64 IOTimerGetTicks();