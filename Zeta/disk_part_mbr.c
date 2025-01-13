#include "disk_part_mbr.h"

#include "debugger.h"
#include "utils.h"

#define READ_(tmp_ret, tmp_len, tmp_i, src, len)                        \
    ({                                                                  \
        ZETA_AutoVar(tmp_len, (len));                                   \
                                                                        \
        for (size_t tmp_i = 0; tmp_i < tmp_len; ++tmp_i) {              \
            if (src[tmp_i] < 0 || 255 < src[tmp_i]) { goto ERR_RET; }   \
        }                                                               \
                                                                        \
        u64_t tmp_ret =                                                 \
            ZETA_BytesToUInt(u64_t, (src), tmp_len, ZETA_LittleEndian); \
                                                                        \
        (src) += tmp_len;                                               \
                                                                        \
        tmp_ret;                                                        \
    })

#define READ(src, len) \
    READ_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, (src), (len))

#define WRITE_(tmp_len, val, dst, len)                          \
    ZETA_AutoVar(tmp_len, (len));                               \
    ZETA_UIntToBytes((val), (dst), tmp_len, ZETA_LittleEndian); \
    (dst) += tmp_len;                                           \
    ZETA_StaticAssert(TRUE)

#define WRITE(val, dst, len) WRITE_(ZETA_TmpName, (val), (dst), (len))

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

ERR_RET:

    return NULL;
}

static byte_t* WritePartInfo_(byte_t* dst,
                              Zeta_DiskPartMBR_PartEntry const* part_entry) {
    WRITE(part_entry->state, dst, 1);

    WRITE(0xFFFFFE, dst, 3);

    WRITE(part_entry->type, dst, 1);

    WRITE(0xFFFFFE, dst, 3);

    WRITE(part_entry->beg, dst, 4);

    WRITE(part_entry->end - part_entry->beg, dst, 4);

    return dst;
}

byte_t const* Zeta_DiskPartMBR_ReadMBR(Zeta_DiskPartMBR_MBR* dst,
                                       Zeta_DiskInfo const* disk_info,
                                       byte_t const* data, size_t data_size) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(disk_info != NULL);
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(512 <= data_size);

    for (int i = 0; i < ZETA_DiskPartMBR_size_of_bootstrap; ++i, ++data) {
        dst->bootstrap[i] = data[0];
    }

    for (int i = 0; i < 4; ++i) {
        data = ReadPartInfo_(dst->part_entries + i, data);
    }

    if (data[0] != 0x55 || data[1] != 0xAA) { return NULL; }

    return data + 2;
}

byte_t* Zeta_DiskPartMBR_WriteMBR(byte_t* dst, size_t dst_size,
                                  Zeta_DiskInfo const* disk_info,
                                  Zeta_DiskPartMBR_MBR* mbr) {
    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(512 <= dst_size);
    ZETA_DebugAssert(disk_info != NULL);
    ZETA_DebugAssert(mbr != NULL);

    for (int i = 0; i < ZETA_DiskPartMBR_size_of_bootstrap; ++i, ++dst) {
        dst[0] = mbr->bootstrap[i];
    }

    for (int i = 0; i < 4; ++i) {
        dst = WritePartInfo_(dst, mbr->part_entries + i);
    }

    WRITE(0xAA55, dst, 2);

    return dst;
}
