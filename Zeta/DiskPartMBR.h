#pragma once

#include "define.h"

ZETA_extern_c_beg;

#define ZETA_DiskParMBR_size_of_bootstrap (446)

    typedef struct Zeta_DiskParMBR_PartEntry Zeta_DiskParMBR_PartEntry;

struct Zeta_DiskParMBR_PartEntry {
    byte_t state;
    u32_t beg_chs[3];
    u32_t end_chs[3];
    byte_t type;
    u32_t lba_beg;
    u32_t num_of_secs;
};

typedef struct Zeta_DiskParMBR_MBR Zeta_DiskParMBR_MBR;

struct Zeta_DiskParMBR_MBR {
    byte_t bootstrap[ZETA_DiskParMBR_size_of_bootstrap];
    Zeta_DiskParMBR_PartEntry part_entries[4];
    size_t boot_sign;
};

const byte_t* Zeta_DiskPartMBR_ReadMBR(Zeta_DiskParMBR_MBR* dst,
                                       const byte_t* data);

byte_t* Zeta_DiskPartMBR_WriteMBR(byte_t* dst, Zeta_DiskParMBR_MBR* mbr);

ZETA_extern_c_end;
