// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <termOS.h>

typedef struct OBJCSelector    OBJCSelector;
typedef struct OBJCMethod      OBJCMethod;
typedef struct OBJCMethodList  OBJCMethodList;
typedef struct OBJCClass       OBJCClass;
typedef struct OBJCObject      OBJCObject;
typedef struct OBJCSymbolTable OBJCSymbolTable;
typedef struct OBJCModule      OBJCModule;

struct OBJCSelector {
    const char* name;
    const char* types;
};

struct OBJCMethod {
    struct OBJCSelector* selector;
    const char* types;
    void* functionPointer; // pointer to function
};

struct OBJCMethodList {
    struct OBJCMethodList* next;
    Int32 methodCount;
    struct OBJCMethod methods[];
};

struct OBJCClass {
    OBJCClass* metaClass; // pointer to metaclass
    OBJCClass* parentClass; // parent
    const char* name;
    Int64 version;
    Int64 info;
    Int64 instanceSize;
    OBJCMethodList* methods;
};

struct OBJCObject {
    OBJCClass* classPointer;
};

struct OBJCSymbolTable {
    UInt32 selectorReferencesCount;
    OBJCSelector* selectorReferences;
    UInt16 classDefinitionCount;
    UInt16 categoryDefinitionCount;
    void* definitions[];
};

struct OBJCModule {
    UInt64 version;
    UInt64 size;
    const char* name;
    OBJCSymbolTable* symbolTable;
};

enum { 
    kOBJCRuntimeMaxClasses = 256
};

static OBJCClass* _classTable[kOBJCRuntimeMaxClasses];
static UInt32 _classCount = 0;

static OBJCClass* sOBJCGetClass(const char* name) {
    for (UInt32 i = 0; i < _classCount; i++) {
        if (StringCompare(_classTable[i]->name, name) == 0) return _classTable[i];
    }
    return nullptr;
} 

static void sOBJCRegisterModule(OBJCModule* module) {
    if (!module || !module->symbolTable) return;
    OBJCSymbolTable* symbolTable = module->symbolTable;
    for (UInt16 i = 0; i < symbolTable->classDefinitionCount; i++) {
        OBJCClass* class = (OBJCClass*)symbolTable->definitions[i];
        if (_classCount < kOBJCRuntimeMaxClasses) {
            _classTable[_classCount++] = class;
        }
    }
}

// real

extern void* __objc_module_list_start;
extern void* __objc_module_list_end;

OBJCClass* objc_getClass(const char* name) {
    return sOBJCGetClass(name);
}

void __objc_exec_class(OBJCModule* module) {
    sOBJCRegisterModule(module);
}

OBJCClass* objc_lookup_class(const char* name) {
    return sOBJCGetClass(name);
}

void _objc_init_runtime() {
    void** currentModule = &__objc_module_list_start;
    while (currentModule < &__objc_module_list_end) {
        if (*currentModule) __objc_exec_class((OBJCModule*)*currentModule);
        currentModule++;
    }
}


OBJCObject* OBJCAllocateInstance(OBJCClass* class) {
    if (!class) return nullptr;

    OBJCObject* instance = (OBJCObject*)MemoryAllocate(class->instanceSize);
    if (instance) {
        MemorySet(instance, 0, class->instanceSize);
        instance->classPointer = class;
    }
    
    return instance;
}