#pragma once

#include "define.h"

#define ZETA_Disk_Sec_Flag_read ((u32_t)0b001)
#define ZETA_Disk_Sec_Flag_write ((u32_t)0b010)
#define ZETA_Disk_Sec_Flag_dirty ((u32_t)0b100)

typedef struct Zeta_Disk_Sec Zeta_Disk_Sec;

struct Zeta_Disk_Sec {
    unsigned int flag;
    byte_t* data;
};

typedef struct Zeta_Disk Zeta_Disk;

struct Zeta_Disk {
    void* context;

    size_t (*GetSize)(void* context);

    void (*Read)(void* context, byte_t* dst, size_t beg, size_t end);

    void (*Write)(void* context, size_t beg, size_t end, byte_t const* data);
};
