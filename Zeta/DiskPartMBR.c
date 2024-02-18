#include "DiskPartMBR.h"

#include "utils.h"

static const byte_t* Zeta_ReadPartInfo(Zeta_DiskPartMBR_PartEntry* dst,
                                       const byte_t* data) {
    dst->state = data[0];
    ++data;

    dst->end_chs[0] = data[1] / 64 * 128 + data[2];
    dst->end_chs[1] = data[0];
    dst->end_chs[2] = data[1] % 64;
    data += 3;

    dst->type = data[0];
    ++data;

    dst->end_chs[0] = data[1] / 64 * 128 + data[2];
    dst->end_chs[1] = data[0];
    dst->end_chs[2] = data[1] % 64;
    data += 3;

    dst->lba_beg = Zeta_ReadLittleEndian(data, 4);
    data += 4;

    dst->num_of_secs = Zeta_ReadLittleEndian(data, 4);
    data += 4;

    return data;
}

static byte_t* Zeta_WritePartInfo(
    byte_t* dst, const Zeta_DiskPartMBR_PartEntry* part_entry) {
    dst[0] = part_entry->state;
    ++dst;

    dst[0] = part_entry->beg_chs[1];
    dst[1] = part_entry->beg_chs[0] / 128 * 64 + part_entry->beg_chs[2] % 64;
    dst[2] = part_entry->beg_chs[0] % 128;
    dst += 3;

    dst[0] = (byte_t)part_entry->type;
    ++dst;

    dst[0] = part_entry->end_chs[1];
    dst[1] = part_entry->end_chs[0] / 128 * 64 + part_entry->end_chs[2] % 64;
    dst[2] = part_entry->end_chs[0] % 128;
    dst += 3;

    Zeta_WriteLittleEndian(dst, part_entry->lba_beg, 4);
    dst += 4;

    Zeta_WriteLittleEndian(dst, part_entry->num_of_secs, 4);
    dst += 4;

    return dst;
}

const byte_t* Zeta_DiskPartMBR_ReadMBR(Zeta_DiskPartMBR_MBR* dst,
                                       const byte_t* data,
                                       const byte_t* data_end) {
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

    dst->boot_sign = Zeta_ReadLittleEndian(data, 2);
    data += 2;

    return data;
}

byte_t* Zeta_DiskPartMBR_WriteMBR(byte_t* dst, Zeta_DiskPartMBR_MBR* mbr) {
    for (int i = 0; i < ZETA_DiskPartMBR_size_of_bootstrap; ++i, ++dst) {
        dst[0] = mbr->bootstrap[i];
    }

    for (int i = 0; i < 4; ++i) {
        dst = Zeta_WritePartInfo(dst, mbr->part_entries + i);
    }

    Zeta_WriteLittleEndian(dst, mbr->boot_sign, 2);
    dst += 2;

    return dst;
}
