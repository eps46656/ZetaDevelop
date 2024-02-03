#pragma once

#include "define.h"

typedef struct Zeta_CRCHasher Zeta_CRCHasher;

struct Zeta_CRCHasher {
    unsigned _BitInt(64) div;
    unsigned _BitInt(64) reg;
};

EXTERN_C void Zeta_CRCHasher_Init(void* hasher, unsigned _BitInt(64) div,
                                  unsigned _BitInt(64) reg);

EXTERN_C unsigned _BitInt(64) Zeta_CRCHasher_GetReg(void* hasher);

EXTERN_C void Zeta_CRCHasher_Rotate(void* hasher, const byte_t* data,
                                    size_t size, int reverse);
