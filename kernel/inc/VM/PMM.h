// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include "bootinfo.h"
#include <types.h>

#define kVMPageSize 4096
#define kVMSafeSpaceStartAddress 0x100000
#define kVMBlocksPerByte 8

#define BITMAP_BYTE_INDEX(address)  ((address / kVMPageSize) / kVMBlocksPerByte)
#define BITMAP_BIT_OFFSET(address)  ((address / kVMPageSize) % kVMBlocksPerByte)
#define BITMAP_TEST(bitmap, address)  (bitmap[BITMAP_BYTE_INDEX(address)] & (1 << BITMAP_BIT_OFFSET(address)))
#define BITMAP_SET(bitmap, address)   (bitmap[BITMAP_BYTE_INDEX(address)] |= (1 << BITMAP_BIT_OFFSET(address)))
#define BITMAP_UNSET(bitmap, address) (bitmap[BITMAP_BYTE_INDEX(address)] &= ~(1 << BITMAP_BIT_OFFSET(address)))

typedef struct {
    UInt32 type;
    UInt32 padding;
    UInt64 physicalStart;
    UInt64 virtualStart;
    UInt64 pageCount;
    UInt64 attributes;
} __attribute__((packed)) OSMemoryDescriptor;

typedef enum {
    kOSMemoryTypeReserved,
    kOSMemoryTypeLoaderCode,
    kOSMemoryTypeLoaderData,
    kOSMemoryTypeBootServicesCode,
    kOSMemoryTypeBootServicesData,
    kOSMemoryTypeRuntimeServicesCode,
    kOSMemoryTypeRuntimeServicesData,
    kOSMemoryTypeConventional,
    kOSMemoryTypeUnusable,
    kOSMemoryTypeACPIReclaim,
    kOSMemoryTypeACPINonVolatile,
    kOSMemoryTypeMappedIO,
    kOSMemoryTypeMappedIOPortSpace,
    kOSMemoryTypePalCode,
    kOSMemoryTypePersistent,
    kOSMemoryTypeMax
} OSMemoryType;

UInt64 VMPhysicalMemoryGetTotalMemorySize();
void  VMPhysicalMemoryInitialize(BIMemoryMap* mmap);
void* VMPhysicalMemoryAllocatePage();
void  VMPhysicalMemoryFreePage(void* addr);
