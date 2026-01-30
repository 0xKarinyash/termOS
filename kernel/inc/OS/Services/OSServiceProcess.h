// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once

#include <types.h>

Int32 OSServiceProcessExit(Int32 code);
Int32 OSServiceProcessSpawn(const char* path);
Int32 OSServiceProcessWait(UInt64 pid);