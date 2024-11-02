#include "disk_part_mbt.h"

#include "utils.h"

#define READ(src, length)                      \
    ({                                         \
        Zeta_ReadLittleEndianStd(src, length); \
        src += length;                         \
    })

#define WRITE(dst, val, length)               \
    Zeta_WriteLittleEndian(dst, val, length); \
    dst += length;                            \
    ZETA_StaticAssert(TRUE)

static byte_t const* ReadPartInfo_(Zeta_DiskPartMBR_PartEntry* dst,
                                   byte_t const* src) {
    dst->state = READ(src, 1);

    // ignore chs
    src += 3;

    dst->type = READ(src, 1);

    // ignore chs
    src += 3;

    dst->beg = READ(src, 4);

    size_t num_of_secs = READ(src, 4);

    dst->end = dst->beg + num_of_secs;

    return src;
}

static byte_t* WritePartInfo_(byte_t* dst,
                              Zeta_DiskPartMBR_PartEntry const* part_entry) {
    WRITE(dst, part_entry->state, 1);

    WRITE(dst, 0xFFFFFE, 3);

    WRITE(dst, part_entry->type, 1);

    WRITE(dst, 0xFFFFFE, 3);

    WRITE(dst, part_entry->beg, 4);

    WRITE(dst, part_entry->end - part_entry->beg, 4);

    return dst;
}

byte_t const* Zeta_DiskPartMBR_ReadMBR(Zeta_DiskPartMBR_MBR* dst,
                                       Zeta_DiskInfo const* disk_info,
                                       byte_t const* src,
                                       byte_t const* src_end) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(disk_info != NULL);
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(src_end != NULL);
    ZETA_DebugAssert(src <= src_end);
    ZETA_DebugAssert(512 <= src_end - src);

    for (int i = 0; i < ZETA_DiskPartMBR_size_of_bootstrap; ++i, ++src) {
        dst->bootstrap[i] = src[0];
    }

    for (int i = 0; i < 4; ++i) {
        src = ReadPartInfo_(dst->part_entries + i, src);
    }

    if (src[0] != 0x55 || src[1] != 0xAA) { return NULL; }

    src += 2;
    return src;
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

    WRITE(dst, 0xAA55, 2);

    return dst;
}
