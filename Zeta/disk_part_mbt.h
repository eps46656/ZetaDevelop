#pragma once

#include "disk_info.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DiskPartMBR_MBR);
ZETA_DeclareStruct(Zeta_DiskPartMBR_PartEntry);

#define ZETA_DiskPartMBR_size_of_bootstrap (446)

struct Zeta_DiskPartMBR_PartEntry {
    byte_t state;
    byte_t type;
    u64_t beg;
    u64_t end;
};

struct Zeta_DiskPartMBR_MBR {
    byte_t bootstrap[ZETA_DiskPartMBR_size_of_bootstrap];
    Zeta_DiskPartMBR_PartEntry part_entries[4];
};

byte_t const* Zeta_DiskPartMBR_ReadMBR(Zeta_DiskPartMBR_MBR* dst,
                                       Zeta_DiskInfo const* disk_info,
                                       byte_t const* data,
                                       byte_t const* data_end);

byte_t* Zeta_DiskPartMBR_WriteMBR(byte_t* dst, byte_t* dst_end,
                                  Zeta_DiskInfo const* disk_info,
                                  Zeta_DiskPartMBR_MBR* mbr);

ZETA_ExternC_End;
