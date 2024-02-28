#include "DiskPartGPT.h"

#include "utils.h"

#define READ(dst, length)                          \
    {                                              \
        dst = Zeta_ReadLittleEndian(data, length); \
        data += length;                            \
    }                                              \
    ZETA_StaticAssert(TRUE)

#define WRITE(length, val)                        \
    {                                             \
        Zeta_WriteLittleEndian(dst, length, val); \
        dst += length;                            \
    }                                             \
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

    for (int i = 0; i < 8; ++i, ++data) {
        if (sign[i] != *data) { return NULL; }
    }

    READ(dst->revision_num, 4);
    READ(dst->header_size, 4);
    READ(dst->header_crc32, 4);

    for (int i = 0; i < 4; ++i, ++data) {
        if (*data != 0) { return NULL; }
    }

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

    WRITE(4, header->revision_num);
    WRITE(4, header->header_size);
    WRITE(4, header->header_crc32);

    for (int i = 0; i < 8; ++i, ++dst) { *dst = 0; }

    WRITE(8, header->cur_lba);
    WRITE(8, header->bk_lba);

    WRITE(8, header->first_usable);
    WRITE(8, header->last_usable);

    for (int i = 0; i < 16; ++i, ++dst) { *dst = header->disk_guid[i]; }

    WRITE(8, header->beg_of_part_entries);
    WRITE(4, header->num_of_part_entries);
    WRITE(4, header->size_of_part_entry);
    WRITE(4, header->crc32_of_part_entries);

    return dst;
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

    WRITE(8, part_entry->beg);
    WRITE(8, part_entry->end - 1);

    WRITE(8, part_entry->flags);

    for (int i = 0; i < 72; ++i, ++dst) { *dst = part_entry->name[i]; }

    return dst;
}
