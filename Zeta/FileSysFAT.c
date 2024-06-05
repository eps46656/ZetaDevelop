#include "FileSysFAT.h"

#include "utils.h"

#define READ(dst, length)                          \
    {                                              \
        dst = Zeta_ReadLittleEndian(data, length); \
        data += length;                            \
    }                                              \
    ZETA_StaticAssert(TRUE)

ZETA_DeclareStruct(Header);
ZETA_DeclareStruct(BPB);
ZETA_DeclareStruct(ExtBPB_1216);
ZETA_DeclareStruct(ExtBPB_32);

struct Header {
    unsigned int type;

    unsigned int media;

    byte_t jmp_boot[3];

    byte_t oem_name[8];

    unsigned int num_of_reserved_secs;

    unsigned int bytes_per_sec;

    unsigned int secs_per_clus;

    unsigned int secs_per_trk;

    u32_t num_of_secs;

    unsigned int size_of_fat;

    unsigned int num_of_fats;

    unsigned int num_of_heads;

    unsigned int num_of_hidden_secs;

    u64_t root_clus;

    unsigned int num_of_root_entries;

    u32_t vol_id;

    byte_t vol_lab[11];
};

struct BPB {
    byte_t jmpBoot[3];

    byte_t OEMName[8];

    unsigned int BytsPerSec;

    unsigned int SecPerClus;

    unsigned int RsvdSecCnt;

    unsigned int NumFats;

    unsigned int RootEntCnt;

    unsigned int TotSec16;

    unsigned int Media;

    unsigned int FatSz16;

    unsigned int SecPerTrk;

    unsigned int NumHeads;

    unsigned int HiddSec;

    unsigned int TotSec32;
};

struct ExtBPB_1216 {
    unsigned int DrvNum;

    unsigned int BootSig;

    unsigned int VolID;

    byte_t VolLab[11];

    byte_t FilSysType[8];
};

struct ExtBPB_32 {
    u32_t FatSz32;

    unsigned int ExtFlags;

    unsigned int FSVer;
    unsigned int RootClus;
    unsigned int FSInfo;
    unsigned int BkBootSec;
    unsigned int Reserved;

    unsigned int DrvNum;

    unsigned int BootSig;

    unsigned int VolID;

    byte_t VolLab[11];

    byte_t FilSysType[8];
};

bool_t Zeta_FileSys_FAT_ReadHeader(Header* dst, byte_t const* data) {
    ZETA_DebFileSysst != NULL);
    ZETA_DebugAssert(data != NULL);

#define ASSERT_RET(cond) \
    if (cond) {          \
    } else {             \
        return FALSE;    \
    }

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

    READ(dst->root_ent_cnt, 2);
    // if (dst->num_of_fats == 0) { return FALSE; }

    bool_t is_fat32 = dst->num_of_root_entries == 0;

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

    if (is_fat32) {
        u32_t size_of_fat_32;
        READ(size_of_fat_32, 4);

        if ((size_of_fat_16 == 0) == (size_of_fat_32 == 0)) { return FALSE; }

        dst->size_of_fat = size_of_fat_16 + size_of_fat_32;

        READ(dst->ext_flags, 2);

        READ(dst->fs_ver, 2);

        READ(dst->root_clus, 2);

        READ(dst->fs_info, 2);

        READ(dst->bk_boot_sec, 2);
    } else {
        dst->size_of_fat = size_of_fat_16;

        dst->ext_flags = 0;

        dst->fs_ver = 0;

        dst->root_clus = 0;

        dst->fs_info = 0;

        dst->bk_boot_sec = 0;
    }

    //

    u32_t secs_of_root =
        (dst->root_ent_cnt * 32 + dst->bytes_per_sec - 1) / dst->bytes_per_sec;
}

byte_t const* Zeta_FileSys_FAT_ReadBPB(Zeta_FileSys_FAT_BPB* dst,
                                       byte_t const* data) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(data != NULL);

    for (int i = 0; i < 8; ++i) { dst->jmp_boot[i] = data[i]; }
    data += 3;

    for (int i = 0; i < 8; ++i) { dst->oem_name[i] = data[i]; }
    data += 8;

    READ(dst->byte_per_sec, 2);

    READ(dst->sec_per_clus, 1);

    READ(dst->num_of_reserved_secs, 2);

    READ(dst->num_of_fats, 1);

    data += 2;
    // RootEntCnt

    data += 2;
    // TotSec16

    dst->media = data[0];
    ++data;

    data += 2;
    // FATSz16

    READ(dst->sec_per_trk, 2);

    READ(dst->num_of_heads, 2);

    data += 4;
    // HiddSec

    READ(dst->num_of_secs, 4);

    READ(dst->fat_size, 4);

    READ(dst->ext_flags, 2);

    READ(dst->root_clus, 4);

    READ(dst->root_clus, 4);

    READ(dst->fs_info, 2);

    READ(dst->bk_boot_sec, 2);
}
