#pragma once

#include "Disk.h"
#include "SegVector.h"

typedef struct Zeta_FileSysFAT32_Header Zeta_FileSysFAT32_Header;

struct Zeta_FileSysFAT32_Header {
    u64_t base_idx;

    byte_t jmp_boot[3];

    byte_t oem_name[8];

    u32_t bytes_per_sec;

    u32_t secs_per_clus;

    u32_t num_of_reserved_secs;

    u32_t num_of_fats;

    u32_t num_of_secs;

    byte_t media;

    u32_t size_of_fat;

    u32_t secs_per_trk;

    u32_t num_of_heads;

    u32_t num_of_hidden_secs;

    u32_t ext_flags;

    u32_t fs_ver;

    u64_t root_clus;

    u32_t fs_info;

    u32_t bk_boot_sec;

    u32_t drv_num;

    u32_t vol_id;

    byte_t vol_lab[11];

    byte_t fs_type[8];
};

typedef struct Zeta_FileSysFAT32_DirEntry Zeta_FileSysFAT32_DirEntry;

struct Zeta_FileSysFAT32_DirEntry {
    Zeta_SegVector name;

    u8_t attr;

    u64_t first_clus_num;

    s32_t crt_year;
    u8_t crt_month;
    u8_t crt_day;
    u8_t crt_hour;
    u8_t crt_min;
    u8_t crt_sec;
    u16_t crt_msec;

    s32_t acc_year;
    u8_t acc_month;
    u8_t acc_day;

    s32_t wrt_year;
    u8_t wrt_month;
    u8_t wrt_day;
    u8_t wrt_hour;
    u8_t wrt_min;
    u8_t wrt_sec;
};

typedef struct Zeta_FileSysFAT32_Dir Zeta_FileSysFAT32_Dir;

struct Zeta_FileSysFAT32_Dir {
    Zeta_SegVector entries;
};
