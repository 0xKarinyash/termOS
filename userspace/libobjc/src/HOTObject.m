#import <HOTObject.h>

extern id OBJCAllocateInstance(Class class);
extern void MemoryFree(void* pointer);

@implementation HOTObject
+ (id)alloc {
    return OBJCAllocateInstance((Class)self);
}

- (id)init {
    return self;
}

- (id)free {
    MemoryFree(self);
    return (id)0;
}

@end