#include "DiskPartMBR.h"

#include "utils.h"

#define ASSERT_RET(cond)           \
    if (!(cond)) { goto ERR_RET; } \
    ZETA_StaticAssert(TRUE)

#define READ(dst, length)                      \
    dst = Zeta_ReadLittleEndian(data, length); \
    data += length;                            \
    ZETA_StaticAssert(TRUE)

#define WRITE(val, length)                    \
    Zeta_WriteLittleEndian(dst, val, length); \
    dst += length;                            \
    ZETA_StaticAssert(TRUE)

static byte_t const* ReadPartInfo_(Zeta_DiskPartMBR_PartEntry* dst,
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

static byte_t* WritePartInfo_(byte_t* dst,
                              Zeta_DiskPartMBR_PartEntry const* part_entry) {
    WRITE(part_entry->state, 1);

    WRITE(0xFFFFFE, 3);

    WRITE(part_entry->type, 1);

    WRITE(0xFFFFFE, 3);

    WRITE(part_entry->beg, 4);

    WRITE(part_entry->end - part_entry->beg, 4);

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
        data = ReadPartInfo_(dst->part_entries + i, data);
    }

    ASSERT_RET(data[0] == 0x55 && data[1] == 0xAA);
    data += 2;

    return data;

ERR_RET:
    return NULL;
}

byte_t* Zeta_DiskPartMBR_WriteMBR(byte_t* dst, byte_t* dst_end,
                                  Zeta_DiskInfo const* disk_info,
                                  Zeta_DiskPartMBR_MBR* mbr) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(dst_end != NULL);
    ZETA_DebugAssert(dst <= dst_end);
    ZETA_DebugAssert(512 <= dst_end - dst);
    ZETA_DebugAssert(disk_info != NULL);
    ZETA_DebugAssert(mbr != NULL);

    for (int i = 0; i < ZETA_DiskPartMBR_size_of_bootstrap; ++i, ++dst) {
        dst[0] = mbr->bootstrap[i];
    }

    for (int i = 0; i < 4; ++i) {
        dst = WritePartInfo_(dst, mbr->part_entries + i);
    }

    WRITE(0xAA55, 2);

    return dst;
}
