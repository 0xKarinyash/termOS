typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64; 

#define HOT_MAGIC 0x21544F48

typedef struct hot_segment {
    u64 type; // 1 = rx 2 = rw
    u64 vaddr; 
    u64 offset;
    u64 filesz;
    u64 memsz;
} hot_segment;

typedef struct hot_header {
    u32 magic; // "HOT!"
    u8 version; // 1
    u8 reserved_pad[3];
    u64 entry_point;
    u64 segments_count;
    u64 reserved;
} hot_header;
