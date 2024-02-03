#pragma once

#include "define.h"

ZETA_STATIC_ASSERT(32 <= CHAR_BIT * sizeof(size_t));

typedef struct Zeta_LCGRandomGenerator Zeta_LCGRandomGenerator;

struct Zeta_LCGRandomGenerator {
    size_t a;
    size_t c;
    size_t m;
    size_t seed;
};

EXTERN_C void Zeta_LCGRandomGenerator_Set(void* lcgrg, size_t seed);

EXTERN_C void Zeta_LCGRandomGenerator_Rotate(void* lcgrg);

EXTERN_C size_t Zeta_LCGRandomGenerator_Fetch(void* lcgrg);
