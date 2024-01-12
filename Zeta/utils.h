#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_void_ptr_SIntEx) {
    void* n;
    diff_t i;
};

ZETA_DECL_STRUCT(Zeta_ValOpr) {
    void* context;

    diff_t (*GetSpan)(void* context);

    int (*Compare)(void* context, void* x, void* y);

    void (*Swap)(void* context, void* x, void* y);
};

void Zeta_MemCopy(diff_t size, char* dst, char* src);

void Zeta_MemSwap(diff_t size, char* x, char* y);

void Zeta_Swap(diff_t* a, diff_t* b);

size_t Zeta_PowerM(size_t base, size_t exp, size_t mod);

size_t Zeta_SimpleRandom(size_t x);

size_t Zeta_SimpleHash(size_t x);

void* Zeta_GetMostLink(void* context, void* (*GetLink)(void* context, void* n),
                       void* n);
