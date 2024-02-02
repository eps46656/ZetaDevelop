#pragma once

//

#include "define.h"

#define Zeta_GPT_max_num_of_partition 128

ZETA_DECL_STRUCT(Zeta_DiskPart_GPT) {
    char sign[8];

    size_t revision_num;
    size_t size_of_header;

    size_t crc32_of_header;

    size_t cur_lba;
    size_t backup_lba;

    size_t lba_beg_of_usable_part;
    size_t lba_end_of_usable_part;

    size_t guid;

    size_t lba_beg_of_part_entries;
    size_t num_of_part_entries;
    size_t size_of_part_entries;
    size_t crc32_of_part_entries;
};

ZETA_DECL_STRUCT(Zeta_DiskPart_GPT_PartEntry) {
    u128_t type_guid;
    u128_t part_guid;

    u64_t lba_beg;
    u64_t lba_end;

    u64_t attr;

    char name[72];
};

const byte_t* Zeta_DiskPart_GPT_ReadHeader(Zeta_DiskPart_GPTHeader* dst,
                                           const byte_t* data);
