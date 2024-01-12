#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_Allocator) {
    void* context;

    void* (*Allocate)(void* context, diff_t size);

    void (*Deallocate)(void* context, void* ptr);
};
