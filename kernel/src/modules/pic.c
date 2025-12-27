#include "pic.h"
#include "io.h"
#include "types.h"

#define MASTER_COMMAND 0x20
#define MASTER_DATA    0x21
#define SLAVE_COMMAND  0xA0
#define SLAVE_DATA     0xA1

static inline void send(unsigned short port, unsigned char val) {
    outb(port, val); 
    io_wait();
}

void pic_remap() {
    u64 curr_master = inb(MASTER_DATA);
    u64 curr_slave  = inb(SLAVE_DATA);

    // initialization
    send(MASTER_COMMAND, 0x11);
    send(SLAVE_COMMAND, 0x11);

    send(MASTER_DATA, 0x20); // master now controlling idt[32..39]
    send(SLAVE_DATA, 0x28); // idt[40..47]

    send(MASTER_DATA, 0x04); // slave on irq2
    send(SLAVE_DATA, 0x02); // assign id = 2 to slave

    

    
}