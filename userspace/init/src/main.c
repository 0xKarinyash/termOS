// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <process.h>
#include <stdio.h>
#include <malloc.h>

int main() {
    printf("Launching debug\n");
    spawn("debug");
    while (1) {}
}