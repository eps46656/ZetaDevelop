#pragma once

#include "define.h"

ZETA_extern_c_beg;

#define ZETA_DiskPartMBR_size_of_bootstrap (446)

typedef struct Zeta_DiskPartMBR_PartEntry Zeta_DiskPartMBR_PartEntry;

struct Zeta_DiskPartMBR_PartEntry {
    byte_t state;
    u32_t beg_chs[3];
    u32_t end_chs[3];
    byte_t type;
    u32_t lba_beg;
    u32_t num_of_secs;
};

typedef struct Zeta_DiskPartMBR_MBR Zeta_DiskPartMBR_MBR;

struct Zeta_DiskPartMBR_MBR {
    byte_t bootstrap[ZETA_DiskPartMBR_size_of_bootstrap];
    Zeta_DiskPartMBR_PartEntry part_entries[4];
    size_t boot_sign;
};

const byte_t* Zeta_DiskPartMBR_Read(Zeta_DiskPartMBR_MBR* dst,
                                    const byte_t* data);

byte_t* Zeta_DiskPartMBR_WriteMBR(byte_t* dst, Zeta_DiskPartMBR_MBR* mbr);

ZETA_extern_c_end;
