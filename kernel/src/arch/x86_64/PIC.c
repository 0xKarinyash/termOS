// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <PIC.h>
#include <IO.h>
#include <types.h>


UInt16 HALLegacyInterruptControllerRemap() {
    UInt8 currentMaster = IOPortRead8(kIOMasterData);
    UInt8 currentSlave  = IOPortRead8(kIOSlaveData);

    // initialization; icw1
    IOPortWrite8WithWait(kIOMasterCommand, 0x11);
    IOPortWrite8WithWait(kIOSlaveCommand, 0x11);

    // icw2
    IOPortWrite8WithWait(kIOMasterData, 0x20); // master now controlling idt[32..39]
    IOPortWrite8WithWait(kIOSlaveData, 0x28); // idt[40..47]

    // icw3
    IOPortWrite8WithWait(kIOMasterData, 0x04); // slave on irq2
    IOPortWrite8WithWait(kIOSlaveData, 0x02); // assign id = 2 to slave

    // icw4; 0x01 -- 8086 mode
    IOPortWrite8WithWait(kIOMasterData, 0x01);
    IOPortWrite8WithWait(kIOSlaveData, 0x01); 

    IOPortWrite8WithWait(kIOMasterData, 0xFD); // 1111 1101 
                                  //        0 -- means 'on'
    IOPortWrite8WithWait(kIOSlaveData, 0xFF); // Everything ON

    return ((UInt16) currentMaster << 8) | currentSlave;
}