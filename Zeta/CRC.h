#pragma once

#include "define.h"

ZETA_extern_c_beg;

    typedef struct Zeta_CRCHasher Zeta_CRCHasher;

struct Zeta_CRCHasher {
    unsigned _BitInt(64) div;
    unsigned _BitInt(64) reg;
};

void Zeta_CRCHasher_Init(void* hasher, unsigned _BitInt(64) div,
                         unsigned _BitInt(64) reg);

unsigned _BitInt(64) Zeta_CRCHasher_GetResult(void* hasher);

void Zeta_CRCHasher_Rotate(void* hasher, const byte_t* data, size_t size,
                           int reverse);

ZETA_extern_c_end;
