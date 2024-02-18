#pragma once

#include "define.h"

ZETA_extern_c_beg;

#define ZETA_GPT_max_num_of_partition 128

#define Zeta_DiskPart_GPT_size_of_Header 92

typedef struct Zeta_DiskPart_GPT_Header Zeta_DiskPart_GPT_Header;

struct Zeta_DiskPart_GPT_Header {
    char sign[8];

    size_t revision_num;
    size_t header_size;
    u32_t header_crc32;

    u64_t cur_lba;
    u64_t backup_lba;

    u64_t first_usable;
    u64_t last_usable;

    byte_t disk_guid[16];

    u64_t part_entries_beg;
    u64_t part_entries_num;
    u64_t part_entry_size;
    u64_t part_entries_crc32;
};

#define Zeta_DiskPart_GPT_size_of_PartEntry 128

typedef struct Zeta_DiskPart_GPT_PartEntry Zeta_DiskPart_GPT_PartEntry;

struct Zeta_DiskPart_GPT_PartEntry {
    byte_t type_guid[16];
    byte_t part_guid[16];

    u64_t first_lba;
    u64_t last_lba;

    u64_t flags;

    byte_t name[72];
};

const byte_t* Zeta_DiskPart_GPT_ReadHeader(Zeta_DiskPart_GPT_Header* dst,
                                           const byte_t* data);

byte_t* Zeta_DiskPart_GPT_WriteHeader(byte_t* dst,
                                      Zeta_DiskPart_GPT_Header* header);

const byte_t* Zeta_DiskPart_GPT_ReadPartEntry(Zeta_DiskPart_GPT_PartEntry* dst,
                                              const byte_t* data);

byte_t* Zeta_DiskPart_GPT_WritePartEntry(
    byte_t* dst, Zeta_DiskPart_GPT_PartEntry* part_entry);

ZETA_extern_c_end;
