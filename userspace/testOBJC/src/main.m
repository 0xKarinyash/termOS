// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#import <termOS.h>
#import <HOTObject.h>

@interface Test : HOTObject 
- (void)sayHi;
@end

@implementation Test
- (void)sayHi {
    ConsolePrint("meow");
}
@end

int main() {
    return 1;
    Test *t = [[Test alloc] init];
    [t sayHi];
    return 0;
}