#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_Allocator);

struct Zeta_Allocator {
    void* context;

    size_t (*GetAlign)(void* context);

    size_t (*Query)(void* context, size_t size);

    void* (*Allocate)(void* context, size_t size);

    void (*Deallocate)(void* context, void* ptr);
};

void Zeta_Allocator_Init(void* allocator);

ZETA_ExternC_End;
