#include "DiskPartGPT.h"

#include "utils.h"

#define ASSERT_RET(cond)           \
    if (!(cond)) { goto ERR_RET; } \
    ZETA_StaticAssert(TRUE)

#define READ(src, src_end, length)                      \
    ({                                                  \
        ASSERT_RET(src + length <= src_end);            \
        src += length;                                  \
        Zeta_ReadLittleEndianStd(src - length, length); \
    })

#define WRITE(dst, dst_end, val, length)         \
    ASSERT_RET(dst + length <= dst_end);         \
    Zeta_WriteLittleEndianStd(dst, val, length); \
    dst += length;                               \
    ZETA_StaticAssert(TRUE)

byte_t const* Zeta_DiskPartGPT_ReadHeader(Zeta_DiskPartGPT_Header* dst,
                                          byte_t const* src,
                                          byte_t const* src_end) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(src_end != NULL);
    ZETA_DebugAssert(src <= src_end);
    ZETA_DebugAssert(512 <= src_end - src);

    char const sign[] = "EFI PART";

    for (int i = 0; i < 8; ++i, ++src) { ASSERT_RET(*src == sign[i]); }

    dst->revision_num = READ(src, src_end, 4);
    READ(dst->header_size, src, src_end, 4);
    READ(dst->header_crc32, src, src_end, 4);

    for (int i = 0; i < 4; ++i, ++src) { ASSERT_RET(*src == 0); }

    dst->cur_lba = READ(src, src_end, 8);
    dst->bk_lba = READ(src, src_end, 8);

    dst->first_usable = READ(src, src_end, 8);
    dst->last_usable = READ(src, src_end, 8);

    for (int i = 0; i < 16; ++i, ++src) { dst->disk_guid[i] = *src; }

    dst->beg_of_part_entries = READ(src, src_end, 8);
    dst->num_of_part_entries = READ(src, src_end, 4);
    dst->size_of_part_entry = READ(src, src_end, 4);
    dst->crc32_of_part_entries = READ(src, src_end, 4);

    return src;

ERR_RET:
    return NULL;
}

byte_t* Zeta_DiskPartGPT_WriteHeader(byte_t* dst, byte_t* dst_end,
                                     Zeta_DiskPartGPT_Header* header) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(dst_end != NULL);
    ZETA_DebugAssert(dst <= dst_end);
    ZETA_DebugAssert(512 <= dst_end - dst_end);
    ZETA_DebugAssert(header != NULL);

    char const sign[] = "EFI PART";

    for (int i = 0; i < 8; ++i, ++dst) { *dst = sign[i]; }

    WRITE(header->revision_num, dst, dst_end, 4);
    WRITE(header->header_size, dst, dst_end, 4);
    WRITE(header->header_crc32, dst, dst_end, 4);

    for (int i = 0; i < 8; ++i, ++dst) { *dst = 0; }

    WRITE(header->cur_lba, dst, dst_end, 8);
    WRITE(header->bk_lba, dst, dst_end, 8);

    WRITE(header->first_usable, dst, dst_end, 8);
    WRITE(header->last_usable, dst, dst_end, 8);

    for (int i = 0; i < 16; ++i, ++dst) { *dst = header->disk_guid[i]; }

    WRITE(header->beg_of_part_entries, dst, dst_end, 8);
    WRITE(header->num_of_part_entries, dst, dst_end, 4);
    WRITE(header->size_of_part_entry, dst, dst_end, 4);
    WRITE(header->crc32_of_part_entries, dst, dst_end, 4);

    return dst;

ERR_RET:
    return NULL;
}

byte_t const* Zeta_DiskPartGPT_ReadPartEntry(Zeta_DiskPartGPT_PartEntry* dst,
                                             byte_t const* src,
                                             byte_t const* src_end) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(src_end != NULL);
    ZETA_DebugAssert(128 <= src_end - src);

    for (int i = 0; i < 16; ++i, ++src) { dst->type_guid[i] = *src; }

    for (int i = 0; i < 16; ++i, ++src) { dst->part_guid[i] = *src; }

    dst->beg = READ(src, src_end, 8);
    dst->end = READ(src, src_end, 8) + 1;

    dst->flags = READ(src, src_end, 8);

    for (int i = 0; i < 72; ++i, ++src) { dst->name[i] = *src; }

    return src;

ERR_RET:
    return NULL;
}

byte_t* Zeta_DiskPartGPT_WritePartEntry(
    byte_t* dst, byte_t* dst_end, Zeta_DiskPartGPT_PartEntry* part_entry) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(dst_end != NULL);
    ZETA_DebugAssert(dst <= dst_end);
    ZETA_DebugAssert(128 <= dst_end - dst_end);
    ZETA_DebugAssert(part_entry != NULL);

    for (int i = 0; i < 16; ++i, ++dst) { *dst = part_entry->type_guid[i]; }
    for (int i = 0; i < 16; ++i, ++dst) { *dst = part_entry->part_guid[i]; }

    WRITE(part_entry->beg, dst, dst_end, 8);
    WRITE(part_entry->end - 1, dst, dst_end, 8);

    WRITE(part_entry->flags, dst, dst_end, 8);

    for (int i = 0; i < 72; ++i, ++dst) { *dst = part_entry->name[i]; }

    return dst;

ERR_RET:
    return NULL;
}
