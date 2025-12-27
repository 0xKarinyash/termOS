// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <types.h>

#define MAX(a, b) ({ \
    auto _a = (a);   \
    auto _b = (b);   \
    _a > _b ? _a : _b; \
})

static inline int abs(int n) {
    return (n < 0) ? -n : n;
}

static inline i64 i64abs(i64 n) {
    return (n < 0) ? -n : n;
}

