// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash
#pragma once
#include <types.h>

#define MAX(a, b) ({ \
    auto _a = (a);   \
    auto _b = (b);   \
    _a > _b ? _a : _b; \
})

static inline int abs(int n) {
    return (n < 0) ? -n : n;
}

static inline Int64 i64abs(Int64 n) {
    return (n < 0) ? -n : n;
}

