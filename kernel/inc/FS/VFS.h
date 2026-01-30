// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>

typedef enum FSVNodeFlags {
    kFSVNodeFlagFile      = 0x01,
    kFSVNodeFlagDirectory = 0x02,
} FSVNodeFlags;

typedef struct FSVNode FSVNode;

typedef struct FSVNodeOperations {
    UInt64  (*read)(FSVNode* node, UInt64 offset, UInt64 size, UInt8* buffer);
    UInt64  (*write)(FSVNode* node, UInt64 offset, UInt64 size, UInt8* buffer);
    void    (*open)(FSVNode* node);
    void    (*close)(FSVNode* node);
    FSVNode* (*findDirectory)(FSVNode* node, char* name);
} FSVNodeOperations;

typedef struct FSVNode {
    char                name[256];
    UInt32              protectionMask;
    UInt32              ownerIdentifier;
    UInt32              groupIdentifier;
    UInt32              flags;
    UInt32              inodeIdentifier;
    UInt64              dataLength;
    
    FSVNodeOperations*  operations;
    void*               implementationData; 
    
    struct FSVNode*     mountPoint;
    struct FSVNode*     childNode;
    struct FSVNode*     nextNode; 
} FSVNode;

void    FSVirtualFileSystemInitialize(FSVNode* rootNode);
UInt64  FSVNodeRead(FSVNode* node, UInt64 offset, UInt64 size, UInt8* buffer);
FSVNode* FSVirtualFileSystemOpenPath(const char* path);
void    FSVNodeClose(FSVNode* node);