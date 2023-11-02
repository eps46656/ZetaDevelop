#pragma once

#include "define.h"

struct Zeta_void_ptr_SIntEx {
    void* n;
    diff_t i;
};

void Zeta_Swap(diff_t* a, diff_t* b);

size_t Zeta_PowerM(size_t base, size_t exp, size_t mod);

size_t Zeta_SimpleRandom(size_t x);

size_t Zeta_SimpleHash(size_t x);

void* Zeta_GetMostLink(void* (*GetLink)(void* n), void* n);
