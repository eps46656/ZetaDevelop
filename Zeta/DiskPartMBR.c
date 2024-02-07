#include "DiskPartMBR.h"

#include "utils.h"

#define MASK_6 ((u32_t)0b111111)
#define MASK_8 ((u32_t)0b11111111)

ZETA_StaticAssert(4 <= sizeof(u32_t))

    static const byte_t* Zeta_ReadPartInfo(Zeta_MBRInfo_PartEntry* dst,
                                           const byte_t* data) {
    dst->state = data[0];
    ++data;

    dst->end_chs[0] = ((u32_t)data[1] >> 6 << 8) | (u32_t)data[2];
    dst->end_chs[1] = data[0];
    dst->end_chs[2] = data[1] & MASK_6;
    data += 3;

    dst->type = data[0];
    ++data;

    dst->end_chs[0] = ((u32_t)data[1] >> 6 << 8) | (u32_t)data[2];
    dst->end_chs[1] = data[0];
    dst->end_chs[2] = data[1] & MASK_6;
    data += 3;

    dst->lba_beg = Zeta_ReadLittleEndian(data, 4);
    data += 4;

    dst->num_of_secs = Zeta_ReadLittleEndian(data, 4);
    data += 4;

    return data;
}

static byte_t Zeta_WritePartInfo(byte_t* dst,
                                 const Zeta_DiskParMBR_PartEntry* part_entry) {
    dst[0] = (byte_t)part_entry->state;
    ++dst;

    dst[0] = (byte_t)part_entry->beg_chs[1];
    dst[1] = (byte_t)((part_entry->beg_chs[0] >> 8 << 6) |
                      (part_entry->beg_chs[2] & MASK_6));
    dst[2] = (byte_t)(part_entry->beg_chs[0] & MASK_8);
    dst += 3;

    dst[0] = (byte_t)part_entry->type;
    ++dst;

    dst[0] = (byte_t)part_entry->beg_chs[1];
    dst[1] = (byte_t)((part_entry->beg_chs[0] >> 8 << 6) |
                      (part_entry->beg_chs[2] & MASK_6));
    dst[2] = (byte_t)(part_entry->beg_chs[0] & MASK_8);
    dst += 3;

    Zeta_WriteLittleEndian(dst, part_entry->lba_beg, 4);
    dst += 4;

    Zeta_WriteLittleEndian(dst, part_entry->num_of_secs, 4);
    dst += 4;

    return dst;
}

const byte_t* Zeta_DiskPartMBR_ReadMBR(Zeta_DiskParMBR_MBR* dst,
                                       const byte_t* data) {
    for (int i = 0; i < Zeta_DiskParMBR_size_of_bootstrap; ++i, ++data) {
        dst->bootstrap[i] = data[0];
    }

    for (int i = 0; i < 4; ++i) {
        data = Zeta_ReadPartInfo(dst->part_entries + i, data);
    }

    dst->boot_sign = Zeta_ReadLittleEndian(data, 2);
    data += 2;

    return data;
}

byte_t* Zeta_DiskPartMBR_WriteMBR(byte_t* dst, Zeta_DiskParMBR_MBR* mbr) {
    for (int i = 0; i < Zeta_DiskParMBR_size_of_bootstrap; ++i, ++dst) {
        dst[0] = mbr->bootstrap[i];
    }

    for (int i = 0; i < 4; ++i) {
        dst = Zeta_WritePartInfo(dst, mbr->part_entries + i);
    }

    Zeta_WriteLittleEndian(dst, part_entry->boot_sign, 2);
    dst += 2;

    return dst;
}
