#pragma once

#include "define.h"

ZETA_STATIC_ASSERT(32 <= CHAR_BIT * sizeof(size_t));

ZETA_DECL_STRUCT(Zeta_LCGRandomGenerator) {
    size_t a;
    size_t c;
    size_t m;
    size_t seed;
};

void Zeta_LCGRandomGenerator_Set(void* lcgrg, size_t seed);

void Zeta_LCGRandomGenerator_Rotate(void* lcgrg);

size_t Zeta_LCGRandomGenerator_Fetch(void* lcgrg);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// diff_t Zeta_GetInt
