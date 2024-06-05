#pragma once

#include "define.h"

ZETA_DeclareStruct(Zeta_FileSys_FAT32_BPB);

struct Zeta_FileSys_FAT32_BPB {
    byte_t jmp_boot[3];

    byte_t oem_name[8];

    byte_t media;

    u32_t byte_per_sec;

    u32_t sec_per_clus;

    u32_t num_of_reserved_secs;

    u32_t num_of_fats;

    u32_t sec_per_trk;

    u32_t num_of_heads;

    u32_t num_of_secs;

    u32_t fat_size;

    u16_t ext_flags;

    u32_t root_clus;

    u32_t fs_info;

    u32_t bk_boot_sec;
};
