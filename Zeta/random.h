#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_StaticAssert(32 <= CHAR_BIT * sizeof(size_t));

ZETA_DeclareStruct(Zeta_LCGRandomGenerator);

struct Zeta_LCGRandomGenerator {
    size_t a;
    size_t c;
    size_t m;
    size_t seed;
};

void Zeta_LCGRandomGenerator_Set(void* lcgrg, size_t seed);

void Zeta_LCGRandomGenerator_Rotate(void* lcgrg);

size_t Zeta_LCGRandomGenerator_Fetch(void* lcgrg);

ZETA_ExternC_End;
