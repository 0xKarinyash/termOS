// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <FS/VFS.h>
#include <lib/String.h>

FSVNode* gFSVirtualFileSystemRoot = nullptr;

void FSVirtualFileSystemInitialize(FSVNode* rootNode) {
    gFSVirtualFileSystemRoot = rootNode;
}

UInt64 FSVNodeRead(FSVNode* node, UInt64 offset, UInt64 size, UInt8* buffer) {
    if (!node || !node->operations || !node->operations->read) {
        return 0;
    }

    return node->operations->read(node, offset, size, buffer);
}

FSVNode* FSVirtualFileSystemOpenPath(const char* path) {
    if (!gFSVirtualFileSystemRoot) {
        return nullptr;
    }

    if (strcmp(path, "/") == 0) {
        return gFSVirtualFileSystemRoot;
    }
    
    const char* searchName = path;
    if (path[0] == '/') {
        searchName++;
    }

    FSVNode* nodeIterator = gFSVirtualFileSystemRoot->childNode;
    while (nodeIterator != nullptr) {
        if (strcmp(nodeIterator->name, searchName) == 0) {
            if (nodeIterator->operations && nodeIterator->operations->open) {
                nodeIterator->operations->open(nodeIterator);
            }
            return nodeIterator;
        } 
        nodeIterator = nodeIterator->nextNode;
    }

    return nullptr;
}

void FSVNodeClose(FSVNode* node) {
    if (node && node->operations && node->operations->close) {
        node->operations->close(node);
    }
}