#include "FileSysFAT32.h"

#include "Disk.h"
#include "utils.h"

#define UNUSED_CLUSTER (0)

#define BAD_CLUSTER (0x0FFFFFF7)

#define EOF_CLUSTER (0x0FFFFFFF)

#define ASSERT_RET(cond)           \
    if (!(cond)) { goto ERR_RET; } \
    ZETA_StaticAssert(TRUE)

#define READ(dst, length)                      \
    dst = Zeta_ReadLittleEndian(data, length); \
    data += length;                            \
    ZETA_StaticAssert(TRUE)

typedef struct DirEntry DirEntry;

struct DirEntry {
    bool_t is_long_name;

    union {
        struct {
            byte_t name[11];

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
        } content;

        struct {
            //
        } long_name;
    };
};

bool_t ReadHeader_(Zeta_FileSysFAT32_Header* dst, byte_t const* data) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(data != NULL);

    for (int i = 0; i < 3; ++i) { dst->jmp_boot[i] = data[i]; }
    data += 3;

    for (int i = 0; i < 8; ++i) { dst->oem_name[i] = data[i]; }
    data += 8;

    READ(dst->bytes_per_sec, 2);

    switch (dst->bytes_per_sec) {
        case 512:
        case 1024:
        case 2048:
        case 4096: break;

        default: goto ERR_RET;
    }

    READ(dst->secs_per_clus, 1);

    switch (dst->secs_per_clus) {
        case 1:
        case 2:
        case 4:
        case 8:
        case 16:
        case 32:
        case 64:
        case 128: break;

        default: goto ERR_RET;
    }

    READ(dst->num_of_reserved_secs, 2);
    ASSERT_RET(dst->num_of_reserved_secs != 0);

    READ(dst->num_of_fats, 1);
    ASSERT_RET(dst->num_of_fats != 0);

    u32_t root_ent_cnt;
    READ(root_ent_cnt, 2);
    ASSERT_RET(root_ent_cnt == 0);

    u32_t num_of_secs_16;
    READ(num_of_secs_16, 2);
    ASSERT_RET(num_of_secs_16 == 0 && num_of_secs_16 < 0x10000);

    READ(dst->media, 1);

    switch (dst->media) {
        case 0xF0:
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF: break;

        default: goto ERR_RET;
    }

    u32_t size_of_fat_16;
    READ(size_of_fat_16, 2);

    READ(dst->secs_per_trk, 2);

    READ(dst->num_of_heads, 2);

    READ(dst->num_of_hidden_secs, 4);

    u32_t num_of_secs_32;
    READ(num_of_secs_32, 4);
    ASSERT_RET(num_of_secs_32 == 0 || 0x10000 <= num_of_secs_32);

    ASSERT_RET((num_of_secs_16 == 0) != (num_of_secs_32 == 0));

    dst->num_of_secs = num_of_secs_16 + num_of_secs_32;

    u32_t size_of_fat_32;
    READ(size_of_fat_32, 4);

    ASSERT_RET((size_of_fat_16 == 0) != (size_of_fat_32 == 0));

    dst->size_of_fat = size_of_fat_16 + size_of_fat_32;

    READ(dst->ext_flags, 2);

    READ(dst->fs_ver, 2);

    READ(dst->root_clus, 4);

    READ(dst->fs_info, 2);

    READ(dst->bk_boot_sec, 2);

    // padding
    data += 12;

    READ(dst->drv_num, 1);

    data += 1;

    u32_t boot_sig;
    READ(boot_sig, 1);

    ZETA_Unused(boot_sig);

    READ(dst->vol_id, 4);

    for (int i = 0; i < 11; ++i) { dst->vol_lab[i] = data[i]; }
    data += 11;

    for (int i = 0; i < 8; ++i) { dst->fs_type[i] = data[i]; }
    data += 8;

    data += 420;

    ASSERT_RET(data[0] == 0x55 && data[1] == 0xAA);
    data += 2;

    return data;

ERR_RET:
    return FALSE;
}

static bool_t IsUnusedCluster_(u32_t val) { return val == UNUSED_CLUSTER; }

static bool_t IsBadCluster_(u32_t val) { return val == BAD_CLUSTER; }

static bool_t IsEOFCluster_(u32_t val) {
    return 0x0FFFFFF8 <= val && val <= 0x0FFFFFFF;
}

static u64_t GetFATEntryIdx_(Zeta_FileSysFAT32_Header* header, size_t fat_idx,
                             u32_t clus_num) {
    ZETA_DebugAssert(header != NULL);

    ZETA_DebugAssert(fat_idx < header->num_of_fats);

    u32_t num_of_enties = header->bytes_per_sec * header->size_of_fat / 4;
    ZETA_DebugAssert(2 <= clus_num);
    ZETA_DebugAssert(clus_num < num_of_enties + 2);

    u64_t ret = header->base_idx +
                (u64_t)header->bytes_per_sec * (header->num_of_reserved_secs +
                                                header->size_of_fat * fat_idx) +
                (u64_t)clus_num * 4;

    return ret;
}

static size_t GetFATEntry_(Zeta_FileSysFAT32_Header* header, size_t fat_idx,
                           u32_t clus_num, Zeta_Disk* disk) {
    ZETA_DebugAssert(header != NULL);
    ZETA_DebugAssert(disk != NULL);

    u64_t idx = GetFATEntryIdx_(header, fat_idx, clus_num);

    byte_t tmp[4];
    disk->Read(disk->context, idx, idx + 4, tmp);

    return Zeta_ReadLittleEndian(tmp, 4);
}

static bool_t SetFATEntry_(Zeta_FileSysFAT32_Header* header, size_t fat_idx,
                           u32_t clus_num, u32_t val, Zeta_Disk* disk) {
    ZETA_DebugAssert(header != NULL);
    ZETA_DebugAssert(disk != NULL);

    u64_t idx = GetFATEntryIdx_(header, fat_idx, clus_num);

    u32_t num_of_enties = header->bytes_per_sec * header->size_of_fat / 4;

    u32_t const M = 0x10000000;

    ZETA_DebugAssert(val < 0x10000000);

    if (!IsUnusedCluster_(val) && !IsBadCluster_(val) && !IsEOFCluster_(val)) {
        ZETA_DebugAssert(2 <= val);
        ZETA_DebugAssert(val < num_of_enties + 2);
    }

    byte_t tmp[4];
    disk->Read(disk->context, idx, idx + 4, tmp);

    u32_t cur_val = Zeta_ReadLittleEndian(tmp, 4);
    u32_t nxt_val = cur_val / M * M + val;

    if (cur_val != nxt_val) {
        Zeta_WriteLittleEndian(tmp, 4, nxt_val);
        disk->Write(disk->context, idx, idx + 4, tmp);
    }

    return TRUE;
}
