// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <process.h>
#include <malloc.h>

// TODO: read .cfg and spawn what stated there
int main() {
    while (1) {
        Int32 pid = spawn("/bin/termosh");
        if (pid < 0) return pid;
        wait(pid);
    }
}