// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <Process.h>

extern Int32 OSServiceProcessSpawn(const char* path);
extern Int32 OSServiceProcessWait(UInt64 pid);

Int32 ProcessSpawn(const char* path) {
    return OSServiceProcessSpawn(path);
}

Int32 ProcessWait(UInt64 pid) {
    return OSServiceProcessWait(pid);
}