// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash
#include <process.h>
#include <stdio.h>

void cmd_spawn(const char* path) {
    Int32 pid = spawn(path);
    if (pid < 0) {
        switch (pid) {
            case -1: printf("\"%s\" not found.\n", path); break;
            default: printf("failed to spawn (%d)\n", pid); break;
        }
    } else {
        wait(pid);
    }
}