#pragma once

#include "allocator.h"

ZETA_DeclareStruct(Zeta_PoolAllocator);

struct Zeta_PoolAllocator {
    void* n;
};

void Zeta_PoolAllocator_Init(void* pa);

size_t Zeta_PoolAllocator_GetAlign(void const* pa);

void* Zeta_PoolAllocator_Allocate(void* pa, size_t size);

void Zeta_PoolAllocator_Deallocate(void* pa, void* ptr);

void Zeta_PoolAllocator_DeployAllocator(void* pa, Zeta_Allocator* dst);
