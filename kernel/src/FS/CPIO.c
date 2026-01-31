// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <types.h>
#include <FS/CPIO.h>
#include <FS/VFS.h>
#include <lib/String.h>
#include <OS/OSPanic.h>
#include <VM/Heap.h>
#include <IO/IOConsole.h>

#define FSCPIO_ALIGN4(x) (((x) + 3) & ~3)

typedef struct FSCPIOHeader {
    char magic[6];
    char inode[8];
    char mode[8];
    char userIdentifier[8];
    char groupIdentifier[8];
    char linkCount[8];
    char modificationTime[8];
    char fileLength[8];
    char deviceMajor[8];
    char deviceMinor[8];
    char remoteDeviceMajor[8];
    char remoteDeviceMinor[8];
    char nameLength[8];
    char checkSum[8];
} FSCPIOHeader;

static FSVNodeOperations gFSCPIOOperations = {
    .read = FSCPIORead,
    .write = nullptr,
    .open = nullptr,
    .close = nullptr,
};

static UInt64 sFSCPIOHexadecimalToUInt64(const char* string, Int32 length) {
    UInt64 result = 0;
    for (Int32 i = 0; i < length; i++) {
        char character = string[i];
        result <<= 4;
        if (character >= '0' && character <= '9') result += (character - '0');
        else if (character >= 'A' && character <= 'F') result += (character - 'A' + 10);
        else if (character >= 'a' && character <= 'f') result += (character - 'a' + 10);
    }
    return result;
}

UInt64 FSCPIORead(FSVNode* node, UInt64 offset, UInt64 size, UInt8* buffer) {
    if (offset > node->dataLength) return 0;
    if ((offset + size) > node->dataLength) size = node->dataLength - offset;

    MemoryCopy(buffer, (char*)node->implementationData + offset, size);
    return size;
}

FSVNode* FSCPIOMount(void* baseAddress, UInt64 totalSize) {
    UInt8* currentPointer = (UInt8*)baseAddress;
    UInt8* endPointer = currentPointer + totalSize;

    FSVNode* rootNode = (FSVNode*)VMHeapAllocate(sizeof(FSVNode));
    if (!rootNode) OSPanic("CPIO: Failed to allocate memory for root node");
    
    MemorySet(rootNode, 0, sizeof(FSVNode));
    StringCopy(rootNode->name, "/");
    rootNode->flags = kFSVNodeFlagDirectory;
    rootNode->operations = &gFSCPIOOperations;

    FSVNode* tailNode = nullptr;

    while (currentPointer < endPointer) {
        FSCPIOHeader* header = (FSCPIOHeader*)currentPointer;

        if (StringCompareWithLimit(header->magic, "070701", 6) != 0) {
            OSPanic("CPIO: Invalid magic detected in initramfs");
        }

        UInt64 nameSize = sFSCPIOHexadecimalToUInt64(header->nameLength, 8);
        UInt64 fileSize = sFSCPIOHexadecimalToUInt64(header->fileLength, 8);

        char* fileName = (char*)(currentPointer + sizeof(FSCPIOHeader));
        if (StringCompare(fileName, "TRAILER!!!") == 0) break;

        UInt64 headerAndNameLength = sizeof(FSCPIOHeader) + nameSize;
        UInt64 offsetToData = FSCPIO_ALIGN4(headerAndNameLength);
        void* fileContent = (void*)(currentPointer + offsetToData);

        FSVNode* newNode = (FSVNode*)VMHeapAllocate(sizeof(FSVNode));
        if (!newNode) OSPanic("CPIO: Failed to allocate memory for new node");
        
        MemorySet(newNode, 0, sizeof(FSVNode));
        StringCopyWithLimit(newNode->name, fileName, sizeof(newNode->name) - 1);

        newNode->dataLength = fileSize;
        newNode->inodeIdentifier = sFSCPIOHexadecimalToUInt64(header->inode, 8);
        newNode->operations = &gFSCPIOOperations;
        newNode->implementationData = fileContent;

        UInt64 mode = sFSCPIOHexadecimalToUInt64(header->mode, 8);
        if ((mode & 0xF000) == 0x4000) {
            newNode->flags = kFSVNodeFlagDirectory;
        } else {
            newNode->flags = kFSVNodeFlagFile;
        }

        if (rootNode->childNode == nullptr) {
            rootNode->childNode = newNode;
        } else if (tailNode) {
            tailNode->nextNode = newNode;
        }

        tailNode = newNode;

        IOConsoleLog("^bCPIO^!: Found object '^y%s^!' (size: ^y%d^!) at ^y0x%x^!\n", fileName, fileSize, fileContent);

        UInt64 dataLengthAligned = FSCPIO_ALIGN4(fileSize);
        currentPointer += offsetToData + dataLengthAligned;
    }

    return rootNode;
}