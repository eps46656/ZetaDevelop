#pragma once

#include "allocator.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_PoolAllocator);

struct Zeta_PoolAllocator {
    void* n;
};

void Zeta_PoolAllocator_Init(void* pa);

size_t Zeta_PoolAllocator_GetAlign(void const* pa);

size_t Zeta_PoolAllocator_Query(void const* pa, size_t size);

void* Zeta_PoolAllocator_Allocate(void* pa, size_t size);

void Zeta_PoolAllocator_Deallocate(void* pa, void* ptr);

extern Zeta_Allocator_VTable const zeta_pool_allocator_vtable;

ZETA_ExternC_End;
