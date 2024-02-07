#include "FileSystemFAT32.h"

#include "utils.h"

#define READ(dst, len)                      \
    dst = Zeta_ReadLittleEndian(data, len); \
    data += len;

const byte_t* Zeta_FileSystem_FAT32_ReadBPB(Zeta_FileSystem_FAT32_BPB* dst,
                                            const byte_t* data) {
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
