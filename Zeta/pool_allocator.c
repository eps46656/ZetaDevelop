#include "pool_allocator.h"

#include "debugger.h"

void Zeta_PoolAllocator_Init(void* pa_) {
    Zeta_PoolAllocator* pa = pa_;
    ZETA_DebugAssert(pa != NULL);

    pa->n = NULL;
}

size_t Zeta_PoolAllocator_GetAlign(void* pa_) {
    Zeta_PoolAllocator* pa = pa_;
    ZETA_DebugAssert(pa != NULL);

    return 1;
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

void Zeta_PoolAllocator_DeployAllocator(void* pa_, Zeta_Allocator* dst) {
    Zeta_PoolAllocator* pa = pa_;
    ZETA_DebugAssert(pa != NULL);

    Zeta_Allocator_Init(dst);

    dst->context = pa;

    dst->GetAlign = Zeta_PoolAllocator_GetAlign;

    dst->Allocate = Zeta_PoolAllocator_Allocate;

    dst->Deallocate = Zeta_PoolAllocator_Deallocate;
}
