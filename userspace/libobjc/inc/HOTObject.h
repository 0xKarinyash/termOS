// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <Types.h>

typedef struct OBJCClass  OBJCClass;
typedef struct OBJCObject OBJCObject;

typedef OBJCObject* id;
typedef OBJCClass*  Class;

@interface HOTObject {
    Class classPointer; 
}

+ (id)alloc;
- (id)init;
- (id)free;

@end