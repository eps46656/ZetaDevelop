#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_Cursor);

struct Zeta_Cursor {
    unsigned char data[sizeof(void*) * 8];
} __attribute__((aligned(alignof(max_align_t))));

ZETA_ExternC_End;
