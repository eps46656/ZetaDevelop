#pragma once

#include "define.h"

ZETA_DeclareStruct(Zeta_Disk);
ZETA_DeclareStruct(Zeta_Disk_Sec);

#define ZETA_Disk_Sec_Flag_read ((u32_t)0b001)
#define ZETA_Disk_Sec_Flag_write ((u32_t)0b010)
#define ZETA_Disk_Sec_Flag_dirty ((u32_t)0b100)

struct Zeta_Disk_Sec {
    unsigned flag;
    byte_t* data;
};

struct Zeta_Disk {
    void* context;

    size_t (*GetSize)(void* context);

    void (*Read)(void* context, size_t beg, size_t end, byte_t* dst);

    void (*Write)(void* context, size_t beg, size_t end, byte_t const* src);
};
