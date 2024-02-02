#pragma once

#include "define.h"

#define Zeta_DiskParMBR_size_of_bootstrap (446)

ZETA_DECL_STRUCT(Zeta_DiskParMBR_PartEntry) {
    byte_t state;
    u32_t beg_chs[3];
    u32_t end_chs[3];
    byte_t type;
    u32_t lba_beg;
    u32_t num_of_secs;
};

ZETA_DECL_STRUCT(Zeta_DiskParMBR_MBR) {
    byte_t bootstrap[Zeta_DiskParMBR_size_of_bootstrap];
    Zeta_DiskParMBR_PartEntry part_entries[4];
    size_t boot_sign;
};

const byte_t* Zeta_DiskPartMBR_ReadMBR(Zeta_DiskParMBR_MBR* dst,
                                       const byte_t* data);

byte_t* Zeta_DiskPartMBR_WriteMBR(byte_t* dst, Zeta_DiskParMBR_MBR* mbr);
