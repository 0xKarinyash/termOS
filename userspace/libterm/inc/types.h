// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once

typedef unsigned char      UInt8;
typedef unsigned short     UInt16;
typedef unsigned int       UInt32;
typedef unsigned long long UInt64; 

typedef signed char        Int8;
typedef signed short       Int16;
typedef signed int         Int32;
typedef signed long long   Int64;

typedef UInt64 usize;
typedef UInt64 uintptr_t;

#ifndef __cplusplus
    #define bool _Bool
    #define true 1
    #define false 0
#endif