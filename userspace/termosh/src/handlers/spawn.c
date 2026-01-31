// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash
#include "handlers.h"
#include <termOS.h>

void cmd_spawn(const char* path) {
    Int32 pid = ProcessSpawn(path);
    if (pid < 0) {
        switch (pid) {
            case -1: ConsolePrint("\"%s\" not found.\n", path); break;
            default: ConsolePrint("failed to spawn (%d)\n", pid); break;
        }
    } else {
        ProcessWait(pid);
    }
}