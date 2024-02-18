#include "DiskPartGPT.h"

#include "utils.h"

#define READ(dst, l)                          \
    {                                         \
        dst = Zeta_ReadLittleEndian(data, 4); \
        data += l;                            \
    }                                         \
    ZETA_StaticAssert(TRUE)

#define WRITE(data, l)                        \
    {                                         \
        Zeta_WriteLittleEndian(dst, data, 4); \
        dst += l;                             \
    }                                         \
    ZETA_StaticAssert(TRUE)

const byte_t* Zeta_DiskPart_GPT_ReadHeader(Zeta_DiskPart_GPT_Header* dst,
                                           const byte_t* data) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(data != NULL);

    const char sign[] = "EFI PART";

    for (int i = 0; i < 8; ++i, ++data) {
        if (sign[i] != *data) { return NULL; }
    }

    READ(dst->revision_num, 4);
    READ(dst->header_size, 4);
    READ(dst->header_crc32, 4);

    for (int i = 0; i < 4; ++i, ++data) {
        if (*data != 0) { return 0; }
    }

    READ(dst->cur_lba, 8);
    READ(dst->backup_lba, 8);

    READ(dst->first_usable, 8);
    READ(dst->last_usable, 8);

    for (int i = 0; i < 16; ++i, ++data) { dst->disk_guid[i] = *data; }

    READ(dst->part_entries_beg, 8);
    READ(dst->part_entries_num, 4);
    READ(dst->part_entry_size, 4);
    READ(dst->part_entries_crc32, 4);

    return data;
}

byte_t* Zeta_DiskPart_GPT_WriteHeader(byte_t* dst,
                                      Zeta_DiskPart_GPT_Header* header) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(header != NULL);

    const char sign[] = "EFI PART";

    for (int i = 0; i < 8; ++i, ++dst) { *dst = sign[i]; }

    WRITE(header->revision_num, 4);
    WRITE(header->header_size, 4);
    WRITE(header->header_crc32, 4);

    for (int i = 0; i < 8; ++i, ++dst) { *dst = 0; }

    WRITE(header->cur_lba, 8);
    WRITE(header->backup_lba, 8);

    WRITE(header->first_usable, 8);
    WRITE(header->last_usable, 8);

    for (int i = 0; i < 16; ++i, ++dst) { *dst = header->disk_guid[i]; }

    WRITE(header->part_entries_beg, 8);
    WRITE(header->part_entries_num, 4);
    WRITE(header->part_entry_size, 4);
    WRITE(header->part_entries_crc32, 4);

    return dst;
}

const byte_t* Zeta_DiskPart_GPT_ReadPartEntry(Zeta_DiskPart_GPT_PartEntry* dst,
                                              const byte_t* data) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(data != NULL);

    for (int i = 0; i < 16; ++i, ++data) { dst->type_guid[i] = *data; }

    for (int i = 0; i < 16; ++i, ++data) { dst->part_guid[i] = *data; }

    READ(dst->first_lba, 8);
    READ(dst->last_lba, 8);

    READ(dst->flags, 8);

    for (int i = 0; i < 72; ++i, ++data) { dst->name[i] = *data; }

    return data;
}

byte_t* Zeta_DiskPart_GPT_WritePartEntry(
    byte_t* dst, Zeta_DiskPart_GPT_PartEntry* part_entry) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(part_entry != NULL);

    for (int i = 0; i < 16; ++i, ++dst) { *dst = part_entry->type_guid[i]; }
    for (int i = 0; i < 16; ++i, ++dst) { *dst = part_entry->part_guid[i]; }

    WRITE(part_entry->first_lba, 8);
    WRITE(part_entry->last_lba, 8);

    WRITE(part_entry->flags, 8);

    for (int i = 0; i < 72; ++i, ++dst) { *dst = part_entry->name[i]; }

    return dst;
}
