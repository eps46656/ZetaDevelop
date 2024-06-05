#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_CRCHasher);

struct Zeta_CRCHasher {
    unsigned _BitInt(64) div;
    unsigned _BitInt(64) reg;
};

void Zeta_CRCHasher_Init(void* hasher, unsigned _BitInt(64) div,
                         unsigned _BitInt(64) reg);

unsigned _BitInt(64) Zeta_CRCHasher_GetResult(void* hasher);

void Zeta_CRCHasher_Rotate(void* hasher, byte_t const* data,
                           byte_t const* data_end, int reverse);

ZETA_ExternC_End;
