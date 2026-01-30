// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once

#include <types.h>

Int32 OSLoaderProcessSpawn(const char* executablePath, const char* processName);
void init_task_entry();