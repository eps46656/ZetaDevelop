#include "DiskPartGPT.h"

#include "utils.h"

#define ASSERT_RET(cond)           \
    if (!(cond)) { goto ERR_RET; } \
    ZETA_StaticAssert(TRUE)

#define READ(dst, length)                      \
    ASSERT_RET(length <= data_end - data);     \
    dst = Zeta_ReadLittleEndian(data, length); \
    data += length;                            \
    ZETA_StaticAssert(TRUE)

#define WRITE(val, length)                    \
    ASSERT_RET(length <= dst_end - dst);      \
    Zeta_WriteLittleEndian(dst, val, length); \
    dst += length;                            \
    ZETA_StaticAssert(TRUE)

byte_t const* Zeta_DiskPartGPT_ReadHeader(Zeta_DiskPartGPT_Header* dst,
                                          byte_t const* data,
                                          byte_t const* data_end) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);
    ZETA_DebugAssert(512 <= data_end - data);

    char const sign[] = "EFI PART";

    for (int i = 0; i < 8; ++i, ++data) { ASSERT_RET(*data == sign[i]); }

    READ(dst->revision_num, 4);
    READ(dst->header_size, 4);
    READ(dst->header_crc32, 4);

    for (int i = 0; i < 4; ++i, ++data) { ASSERT_RET(*data == 0); }

    READ(dst->cur_lba, 8);
    READ(dst->bk_lba, 8);

    READ(dst->first_usable, 8);
    READ(dst->last_usable, 8);

    for (int i = 0; i < 16; ++i, ++data) { dst->disk_guid[i] = *data; }

    READ(dst->beg_of_part_entries, 8);
    READ(dst->num_of_part_entries, 4);
    READ(dst->size_of_part_entry, 4);
    READ(dst->crc32_of_part_entries, 4);

    return data;

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

    WRITE(header->revision_num, 4);
    WRITE(header->header_size, 4);
    WRITE(header->header_crc32, 4);

    for (int i = 0; i < 8; ++i, ++dst) { *dst = 0; }

    WRITE(header->cur_lba, 8);
    WRITE(header->bk_lba, 8);

    WRITE(header->first_usable, 8);
    WRITE(header->last_usable, 8);

    for (int i = 0; i < 16; ++i, ++dst) { *dst = header->disk_guid[i]; }

    WRITE(header->beg_of_part_entries, 8);
    WRITE(header->num_of_part_entries, 4);
    WRITE(header->size_of_part_entry, 4);
    WRITE(header->crc32_of_part_entries, 4);

    return dst;

ERR_RET:
    return NULL;
}

byte_t const* Zeta_DiskPartGPT_ReadPartEntry(Zeta_DiskPartGPT_PartEntry* dst,
                                             byte_t const* data,
                                             byte_t const* data_end) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(128 <= data_end - data);

    for (int i = 0; i < 16; ++i, ++data) { dst->type_guid[i] = *data; }

    for (int i = 0; i < 16; ++i, ++data) { dst->part_guid[i] = *data; }

    READ(dst->beg, 8);
    READ(dst->end, 8);
    ++dst->end;

    READ(dst->flags, 8);

    for (int i = 0; i < 72; ++i, ++data) { dst->name[i] = *data; }

    return data;

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

    WRITE(part_entry->beg, 8);
    WRITE(part_entry->end - 1, 8);

    WRITE(part_entry->flags, 8);

    for (int i = 0; i < 72; ++i, ++dst) { *dst = part_entry->name[i]; }

    return dst;

ERR_RET:
    return NULL;
}
