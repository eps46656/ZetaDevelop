#include "pool_allocator.h"

#include "debugger.h"

void Zeta_PoolAllocator_Init(void* pa_) {
    Zeta_PoolAllocator* pa = pa_;
    ZETA_DebugAssert(pa != NULL);

    pa->n = NULL;
}

size_t Zeta_PoolAllocator_GetAlign(void const* pa_) {
    Zeta_PoolAllocator const* pa = pa_;
    ZETA_DebugAssert(pa != NULL);

    return 1;
}

size_t Zeta_PoolAllocator_Query(void const* pa_, size_t size) {
    Zeta_PoolAllocator const* pa = pa_;
    ZETA_DebugAssert(pa != NULL);

    return size;
}

void* Zeta_PoolAllocator_Allocate(void* pa_, size_t size) {
    Zeta_PoolAllocator* pa = pa_;
    ZETA_DebugAssert(pa != NULL);

    if (pa->n == NULL || size == 0) { return NULL; }

    void** n = pa->n;
    pa->n = *n;
    *n = NULL;

    return n;
}

void Zeta_PoolAllocator_Deallocate(void* pa_, void* ptr) {
    Zeta_PoolAllocator* pa = pa_;
    ZETA_DebugAssert(pa != NULL);

    if (ptr == NULL) { return; }

    void** n = ptr;
    *n = pa->n;
    pa->n = n;
}

Zeta_Allocator_VTable const zeta_pool_allocator_vtable = {
    .GetAlign = Zeta_PoolAllocator_GetAlign,

    .Query = Zeta_PoolAllocator_Query,

    .Allocate = Zeta_PoolAllocator_Allocate,

    .Deallocate = Zeta_PoolAllocator_Deallocate,
};
