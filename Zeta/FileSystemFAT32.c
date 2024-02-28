#include "FileSystemFAT32.h"

#include "Disk.h"
#include "utils.h"

#define BAD_CLUSTER 0xFFFFFF7

#define UNUSED_CLUSTER

#define READ(dst, len)                             \
    {                                              \
        dst = Zeta_ReadLittleEndian(data, length); \
        data += length;                            \
    }                                              \
    ZETA_StaticAccert(TRUE)

typedef struct Header Header;

struct Header {
    u64_t base_lba;

    byte_t media;

    byte_t jmp_boot[3];

    byte_t oem_name[8];

    u32_t num_of_reserved_secs;

    u32_t bytes_per_sec;

    u32_t secs_per_clus;

    u32_t secs_per_trk;

    u32_t num_of_secs;

    u32_t size_of_fat;

    u32_t num_of_fats;

    unsigned int num_of_heads;

    unsigned int num_of_hidden_secs;

    u64_t root_clus;

    unsigned int num_of_root_entries;

    u32_t vol_id;

    byte_t vol_lab[11];
};

bool_t ReadHeader_(Header* dst, byte_t const* data) {
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

        default: return FALSE;
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

        default: return FALSE;
    }

    READ(dst->num_of_reserved_secs, 2);
    if (dst->num_of_reserved_secs == 0) { return FALSE; }

    READ(dst->num_of_fats, 1);
    if (dst->num_of_fats == 0) { return FALSE; }

    unsigned int root_ent_cnt;
    READ(root_ent_cnt, 2);
    if (root_ent_cnt != 0) { return FALSE; }

    u32_t num_of_secs_16;
    READ(num_of_secs_16, 2);
    if (num_of_secs_16 != 0 && 0x10000 <= num_of_secs_16) { return FALSE; }

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

        default: return FALSE;
    }

    u32_t size_of_fat_16;
    READ(size_of_fat_16, 2);

    READ(dst->secs_per_trk, 2);

    READ(dst->num_of_heads, 2);

    READ(dst->num_of_hidden_secs, 4);

    u32_t num_of_secs_32;
    READ(num_of_secs_32, 4);
    if (num_of_secs_32 != 0 && num_of_secs_32 < 0x10000) { return FALSE; }

    if ((num_of_secs_16 == 0) == (num_of_secs_32 == 0)) { return FALSE; }

    dst->num_of_secs = num_of_secs_16 + num_of_secs_32;

    u32_t size_of_fat_32;
    READ(size_of_fat_32, 4);

    if ((size_of_fat_16 == 0) == (size_of_fat_32 == 0)) { return FALSE; }

    dst->size_of_fat = size_of_fat_16 + size_of_fat_32;

    READ(dst->ext_flags, 2);

    READ(dst->fs_ver, 2);

    READ(dst->root_clus, 4);

    READ(dst->fs_info, 2);

    READ(dst->bk_boot_sec, 2);

    data += 12;

    READ(dst->drv_num, 1);

    data += 1;

    unsigned int boot_sig;
    READ(boot_sig, 1);

    // unsigned int check_boot_sig = 0;

    READ(dst->vol_id, 4);

    for (int i = 0; i < 11; ++i) { dst->vol_lab[i] = data[i]; }
    data += 11;

    for (int i = 0; i < 8; ++i) { dst->fs_type[i] = data[i]; }
    data += 8;

    data += 420;

    if (data[0] != 0x55 || data[1] != 0xAA) { return FALSE; }
}

void GetFATEntry_(Header* header, size_t fat_idx, size_t entry_num,
                  Zeta_Disk* disk) {
    ZETA_DebugAsssert(header != NULL);
    ZETA_DebugAsssert(disk != NULL);

    ZETA_DebugAssert(fat_idx < header->num_of_fats);

    size_t num_of_enties = header->bytes_per_sec * header->size_of_fat / 4;
    ZETA_DebugAssert(2 <= entry_num);
    ZETA_DebugAssert(entry_num < num_of_enties + 2);

    size_t sec_off = (entry_num - 2) / (header->bytes_per_sec / 4);
    size_t entry_off = (entry_num - 2) % (header->bytes_per_sec / 4);

    size_t sec_lba = header->base_lba + header->num_of_reserved_secs +
                     header->size_of_fat * fat_idx + sec_off;

    byte_t* buff = disk->ReadSec(disk->context, sec_lba);

    Zeta_Disk_Sec sec = disk->Access(disk->context, sec_lba);
    if ((u32_t)sec.flag & ZETA_Disk_Sec_Flag_read == 0) { return FALSE; }

    u32_t ret = Zeta_ReadLittleEndian(buff + entry_off * 4, 4);

    return ret;
}

bool_t SetFATEntry_(Header* header, size_t fat_idx, size_t entry_num,
                    size_t val, Zeta_Disk* disk) {
    ZETA_DebugAsssert(header != NULL);
    ZETA_DebugAsssert(disk != NULL);

    ZETA_DebugAssert(fat_idx < header->num_of_fats);

    size_t num_of_enties = header->bytes_per_sec * header->size_of_fat / 4;
    ZETA_DebugAssert(2 <= entry_num);
    ZETA_DebugAssert(entry_num < num_of_enties + 2);

    size_t sec_off = (entry_num - 2) / (header->bytes_per_sec / 4);
    size_t entry_off = (entry_num - 2) % (header->bytes_per_sec / 4);

    size_t sec_lba = header->base_lba + header->num_of_reserved_secs +
                     header->size_of_fat * fat_idx + sec_off;

    byte_t* buff = disk->ReadSec(disk->context, sec_lba);

    Zeta_Disk_Sec sec = disk->Access(disk->context, sec_lba);
    if ((u32_t)sec.flag & ZETA_Disk_Sec_Flag_write == 0) { return FALSE; }

    Zeta_WriteLittleEndian(sec.data + entry_off * 4, val, 4);

    sec.flag = (u32_t)sec.flag | ZETA_Disk_Sec_Flag_dirty;
    disk->SetFlag(disk->context, sec_lba, sec.flag);
}
