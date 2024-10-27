#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_PtrSizePair);

struct Zeta_PtrSizePair {
    void* ptr;
    size_t size;
};

unsigned long long Zeta_PtrSizePair_Hash(void* context,
                                         void const* ptr_size_pair);

int Zeta_PtrSizePair_Compare(void* context, void const* ptr_size_pair_a,
                             void const* ptr_size_pair_b);

int Zeta_PtrSizePair_PtrCompare(void* context, void const* ptr_size_pair,
                                void const* ptr);

ZETA_ExternC_End;
