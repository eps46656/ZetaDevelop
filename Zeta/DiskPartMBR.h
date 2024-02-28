#pragma once

#include "DiskInfo.h"

ZETA_extern_c_beg;

#define ZETA_DiskPartMBR_size_of_bootstrap (446)

typedef struct Zeta_DiskPartMBR_PartEntry Zeta_DiskPartMBR_PartEntry;

struct Zeta_DiskPartMBR_PartEntry {
    byte_t state;
    byte_t type;
    u64_t beg;
    u64_t end;
};

typedef struct Zeta_DiskPartMBR_MBR Zeta_DiskPartMBR_MBR;

struct Zeta_DiskPartMBR_MBR {
    byte_t bootstrap[ZETA_DiskPartMBR_size_of_bootstrap];
    Zeta_DiskPartMBR_PartEntry part_entries[4];
};

byte_t const* Zeta_DiskPartMBR_ReadMBR(Zeta_DiskPartMBR_MBR* dst,
                                       Zeta_DiskInfo const* disk_info,
                                       byte_t const* data,
                                       byte_t const* data_end);

byte_t* Zeta_DiskPartMBR_WriteMBR(byte_t* dst, Zeta_DiskInfo const* disk_info,
                                  Zeta_DiskPartMBR_MBR* mbr);

ZETA_extern_c_end;
