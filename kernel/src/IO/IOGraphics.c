// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash
// RIP shitgui -- you started it all

#include <OS/OSScheduler.h>
#include <OS/OSPanic.h>
#include <IO/IOGraphics.h>
#include <VM/Heap.h>
#include <lib/String.h>

enum {
    kIOGraphicsTransparencyKey = 0xFF00FF
};

UInt32 IOGraphicsGetPixel(IOGraphicsContext* context, IOGraphicsPoint* point) {
    if (!context->framebuffer || !point || point->x >= context->dimensions.width || point->y >= context->dimensions.height) return 0;
    return context->framebuffer[point->y * context->pixelsPerScanLine + point->x];
}

void IOGraphicsDrawImage(IOGraphicsContext* context, IOGraphicsPoint* point, IOGraphicsImage* image) {
    if (!context->framebuffer || !image) return;

    UInt32 startX = point->x;
    UInt32 startY = point->y;
    UInt32 drawW = image->size.width;
    UInt32 drawH = image->size.height;

    if (startX >= context->dimensions.width || startY >= context->dimensions.height) return;
    if (startX + drawW > context->dimensions.width) drawW = context->dimensions.width - startX;
    if (startY + drawH > context->dimensions.height) drawH = context->dimensions.height - startY;

    for (UInt32 y = 0; y < drawH; y++) {
        volatile UInt32 *destinationPointer = &context->framebuffer[(startY + y) * context->pixelsPerScanLine + startX];
        UInt32 *sourcePointer = &image->buffer[y * image->size.width];
        for (UInt32 x = 0; x < drawW; x++) {
            UInt32 color = sourcePointer[x];
            if (color != kIOGraphicsTransparencyKey) destinationPointer[x] = color;
        }
    }
}

void IOGraphicsDrawRectangle(IOGraphicsContext* context, IOGraphicsPoint* point, IOGraphicsSize* size, IOGraphicsColor color) {
    if (!context->framebuffer) return;

    UInt32 startX = point->x;
    UInt32 startY = point->y;
    UInt32 drawW = size->width;
    UInt32 drawH = size->height;

    if (startX >= context->dimensions.width || startY >= context->dimensions.height) return;
    if (startX + drawW > context->dimensions.width) drawW = context->dimensions.width - startX;
    if (startY + drawH > context->dimensions.height) drawH = context->dimensions.height - startY;


    for (UInt32 y = 0; y < drawH; y++) {
        volatile UInt32* rowPointer = &context->framebuffer[(startY + y) * context->pixelsPerScanLine + startX];
        for (UInt32 x = 0; x < drawW; x++) rowPointer[x] = color;
    }
}

void IOGraphicsDrawCharacter(IOGraphicsContext* context, IOGraphicsPoint* point, char c, IOGraphicsColor color, IOGraphicsFont* font) {
    if (!context->framebuffer) return;

    UInt32 startX = point->x;
    UInt32 startY = point->y;
    UInt32 drawW = font->size.width;
    UInt32 drawH = font->size.height;

    if (startX >= context->dimensions.width || startY >= context->dimensions.height) return;
    if (startX + drawW > context->dimensions.width) drawW = context->dimensions.width - startX;
    if (startY + drawH > context->dimensions.height) drawH = context->dimensions.height - startY;

    for (UInt32 y = 0; y < drawH; y++) {
        UInt8 bitmapRow = font->baseAddress[(unsigned char)c * font->size.height + y];
        volatile UInt32 *rowPointer = &context->framebuffer[(startY + y) * context->pixelsPerScanLine + startX];
        for (UInt32 x = 0; x < drawW; x++) {
            if (bitmapRow & (1 << (7 - x))) {
                rowPointer[x] = color;
            }
        }
    }
}