#pragma once

#include "define.h"

ZETA_extern_c_beg;

#define ZETA_GPT_max_num_of_partition 128

#define Zeta_DiskPartGPT_size_of_Header 92

typedef struct Zeta_DiskPartGPT_Header Zeta_DiskPartGPT_Header;

struct Zeta_DiskPartGPT_Header {
    u32_t revision_num;
    u32_t header_size;
    u32_t header_crc32;

    u64_t cur_lba;
    u64_t bk_lba;

    u64_t first_usable;
    u64_t last_usable;

    byte_t disk_guid[16];

    u64_t beg_of_part_entries;
    u64_t num_of_part_entries;
    u64_t size_of_part_entry;
    u64_t crc32_of_part_entries;
};

#define Zeta_DiskPartGPT_size_of_PartEntry 128

typedef struct Zeta_DiskPartGPT_PartEntry Zeta_DiskPartGPT_PartEntry;

struct Zeta_DiskPartGPT_PartEntry {
    byte_t type_guid[16];
    byte_t part_guid[16];

    u64_t beg;
    u64_t end;

    u64_t flags;

    byte_t name[72];
};

byte_t const* Zeta_DiskPartGPT_ReadHeader(Zeta_DiskPartGPT_Header* dst,
                                          byte_t const* data,
                                          byte_t const* data_end);

byte_t* Zeta_DiskPartGPT_WriteHeader(byte_t* dst, byte_t* dst_end,
                                     Zeta_DiskPartGPT_Header* header);

byte_t const* Zeta_DiskPartGPT_ReadPartEntry(Zeta_DiskPartGPT_PartEntry* dst,
                                             byte_t const* data,
                                             byte_t const* data_end);

byte_t* Zeta_DiskPartGPT_WritePartEntry(byte_t* dst, byte_t* dst_end,
                                        Zeta_DiskPartGPT_PartEntry* part_entry);

ZETA_extern_c_end;
