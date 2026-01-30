// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <process.h>
#include <stdio.h>
#include <malloc.h>

int main() {
    wait(spawn("debug"));
    printf("\nStill here?\n");
    while (1) {
        printf("1");
    }
}