#include "DiskPartMBR.h"

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

static byte_t const* Zeta_ReadPartInfo(Zeta_DiskPartMBR_PartEntry* dst,
                                       byte_t const* data) {
    READ(dst->state, 1);

    // ignore chs
    data += 3;

    READ(dst->type, 1);

    // ignore chs
    data += 3;

    READ(dst->beg, 4);

    size_t num_of_secs;
    READ(num_of_secs, 4);

    dst->end = dst->beg + num_of_secs;

    return data;
}

static byte_t* Zeta_WritePartInfo(
    byte_t* dst, Zeta_DiskPartMBR_PartEntry const* part_entry) {
    WRITE(1, part_entry->state);

    WRITE(3, 0xFFFFFE);

    WRITE(1, part_entry->type);

    WRITE(3, 0xFFFFFE);

    WRITE(4, part_entry->beg);

    WRITE(4, part_entry->end - part_entry->beg);

    return dst;
}

byte_t const* Zeta_DiskPartMBR_ReadMBR(Zeta_DiskPartMBR_MBR* dst,
                                       Zeta_DiskInfo const* disk_info,
                                       byte_t const* data,
                                       byte_t const* data_end) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(disk_info != NULL);
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(data_end != NULL);
    ZETA_DebugAssert(data <= data_end);
    ZETA_DebugAssert(512 <= data_end - data);

    for (int i = 0; i < ZETA_DiskPartMBR_size_of_bootstrap; ++i, ++data) {
        dst->bootstrap[i] = data[0];
    }

    for (int i = 0; i < 4; ++i) {
        data = Zeta_ReadPartInfo(dst->part_entries + i, data);
    }

    if (data[0] != 0x55 || data[1] != 0xAA) { return NULL; }
    data += 2;

    return data;
}

byte_t* Zeta_DiskPartMBR_WriteMBR(byte_t* dst, Zeta_DiskInfo const* disk_info,
                                  Zeta_DiskPartMBR_MBR* mbr) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(disk_info != NULL);
    ZETA_DebugAssert(mbr != NULL);

    for (int i = 0; i < ZETA_DiskPartMBR_size_of_bootstrap; ++i, ++dst) {
        dst[0] = mbr->bootstrap[i];
    }

    for (int i = 0; i < 4; ++i) {
        dst = Zeta_WritePartInfo(dst, mbr->part_entries + i);
    }

    WRITE(2, 0xAA55);

    return dst;
}
