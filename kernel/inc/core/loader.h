// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once

#include <types.h>
#include <core/scheduler.h>

bool exec_init(process* p, const char* path);
void init_task_entry();