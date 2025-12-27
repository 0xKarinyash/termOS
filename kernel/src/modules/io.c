#include "io.h"

// sending junk to unused port letting cpu skipping few tacts.
// not ideal i know.
void io_wait() {
    outb(0x80, 0x0); 
}