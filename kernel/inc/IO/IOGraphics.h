// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash
// RIP shitgui -- you started it all

#pragma once
#include <types.h>

typedef UInt32 IOGraphicsColor;

typedef struct { 
    UInt32 x;
    UInt32 y;
} IOGraphicsPoint;

typedef struct { 
    UInt32 width;
    UInt32 height;
} IOGraphicsSize;

typedef struct {
    volatile UInt32* framebuffer;
    IOGraphicsSize dimensions;
    UInt32 pixelsPerScanLine;
} IOGraphicsContext;

typedef struct {
    IOGraphicsColor* buffer;
    IOGraphicsSize size;
} IOGraphicsImage;

typedef struct {
    IOGraphicsSize size;
    const unsigned char* baseAddress;
} IOGraphicsFont;


UInt32 IOGraphicsGetPixel(IOGraphicsContext* context, IOGraphicsPoint* point);

void IOGraphicsDrawImage(IOGraphicsContext* context, IOGraphicsPoint* point, IOGraphicsImage* image);

void IOGraphicsDrawRectangle(IOGraphicsContext* context, IOGraphicsPoint* point, 
                            IOGraphicsSize* size, IOGraphicsColor color);

void IOGraphicsDrawCharacter(
    IOGraphicsContext* context, 
    IOGraphicsPoint* point, 
    char character, 
    IOGraphicsColor color, 
    IOGraphicsFont* font
);