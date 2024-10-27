#include "Poolallocator.h"

#include "debugger.h"

void Zeta_PoolAllocator_Init(void* pa_) {
    Zeta_PoolAllocator* pa = pa_;
    ZETA_DebugAssert(pa_ != NULL);

    pa->n = NULL;
}

void* Zeta_PoolAllocator_Allocate(void* pa_, size_t size) {
    ZETA_Unused(size);

    Zeta_PoolAllocator* pa = pa_;
    ZETA_DebugAssert(pa_ != NULL);

    if (pa->n == NULL) { return NULL; }

    void** n = pa->n;
    pa->n = *n;
    *n = NULL;

    return n;
}

void Zeta_PoolAllocator_Deallocate(void* pa_, void* ptr) {
    Zeta_PoolAllocator* pa = pa_;

    ZETA_DebugAssert(pa_ != NULL);
    ZETA_DebugAssert(ptr != NULL);

    void** n = ptr;
    *n = pa->n;
    pa->n = n;
}

void Zeta_PoolAllocator_DeployAllocator(void* pa_, Zeta_Allocator* dst) {
    Zeta_PoolAllocator* pa = pa_;

    ZETA_DebugAssert(pa_ != NULL);

    dst->context = pa;
    dst->Allocate = Zeta_PoolAllocator_Allocate;
    dst->Deallocate = Zeta_PoolAllocator_Deallocate;
}
