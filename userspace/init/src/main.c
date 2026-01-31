// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <termOS.h>

// TODO: read .cfg and spawn what stated there
int main() {
    while (1) {
        Int32 pid = ProcessSpawn("/System/CoreServices/termosh");
        if (pid < 0) return pid;
        ProcessWait(pid);
    }
}