typedef unsigned char      UInt8;
typedef unsigned short     UInt16;
typedef unsigned int       UInt32;
typedef unsigned long long UInt64; 

#define HOT_MAGIC 0x21544F48

typedef struct HOTSegment {
    UInt64 type; // 1 = rx 2 = rw
    UInt64 vaddr; 
    UInt64 offset;
    UInt64 filesz;
    UInt64 memsz;
} HOTSegment;

typedef struct HOTHeader {
    UInt32 magic; // "HOT!"
    UInt8 version; // 1
    UInt8 reserved_pad[3];
    UInt64 entry_point;
    UInt64 segments_count;
    UInt64 reserved;
} HOTHeader;
