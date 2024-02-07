#pragma once

#include "define.h"

typedef struct Zeta_Allocator Zeta_Allocator;

struct Zeta_Allocator {
    void* context;

    size_t (*Query)(void* context, size_t size);

    void* (*Allocate)(void* context, size_t size);

    void (*Deallocate)(void* context, void* ptr);
};
