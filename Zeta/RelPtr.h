#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_RelPtr Zeta_RelPtr;

struct Zeta_RelPtr {
    uintptr_t val;
};

void* Zeta_RelPtr_Get(void* rel_ptr);

void Zeta_RelPtr_Set(void* rel_ptr, void* ptr);

ZETA_extern_c_end;
