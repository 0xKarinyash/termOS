#include "rand.h"
#include "types.h"

// really shiity rand lol. 
// not shitty.. basic.

u64 shitrand() {
    int lo, hi;
    __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi)); // why fucking asm refusing to eat my u32

    return ((u64)hi << 32) | lo;
}