
#include <unordered_map>
#include <vector>

#include "../Zeta/allocator.h"

struct ZetaPoolAllocator {
    size_t size;

    size_t max_buffered_ptrs_num;
    std::vector<void*> buffered_ptrs;

    std::unordered_map<unsigned long long, unsigned long long> records;
    size_t usage;
};

size_t ZetaPoolAllocator_GetAlign(void* pa_) {
    ZETA_Unused(pa_);
    return alignof(max_align_t);
}

size_t ZetaPoolAllocator_Query(void* pa_, size_t size) {
    ZetaPoolAllocator* pa = (ZetaPoolAllocator*)pa_;
    ZETA_DebugAssert(pa != NULL);

    if (size == 0) { return 0; }
    if (size <= pa->size) { return pa->size; }
    return 0;
}

void* ZetaPoolAllocator_Allocate(void* pa_, size_t size) {
    ZetaPoolAllocator* pa = (ZetaPoolAllocator*)pa_;
    ZETA_DebugAssert(pa != NULL);

    if (size == 0 || pa->size < size) { return NULL; }

    void* ptr;

    if (!pa->buffered_ptrs.empty()) {
        ptr = pa->buffered_ptrs.back();
        pa->buffered_ptrs.pop_back();
    } else {
        ptr = malloc(pa->size);
    }

#if ZETA_IsDebug
    bool_t b{ pa->records.insert({ ZETA_PtrToAddr(ptr), pa->size }).second };
    ZETA_DebugAssert(b);
    pa->usage += pa->size;
#endif

    return ptr;
}

void ZetaPoolAllocator_Deallocate(void* pa_, void* ptr) {
    ZetaPoolAllocator* pa = (ZetaPoolAllocator*)pa_;
    ZETA_DebugAssert(pa != NULL);

    if (ptr == NULL) { return; }

#if ZETA_IsDebug
    auto iter{ pa->records.find(ZETA_PtrToAddr(ptr)) };
    ZETA_DebugAssert(iter != pa->records.end());
    pa->usage -= iter->second;
    pa->records.erase(iter);
#endif

    if (pa->buffered_ptrs.size() < pa->max_buffered_ptrs_num) {
        pa->buffered_ptrs.push_back(ptr);
    } else {
        free(ptr);
    }
}

size_t ZetaPoolAllocator_GetUsage(const void* pa_) {
    ZetaPoolAllocator* pa = (ZetaPoolAllocator*)pa_;
    ZETA_DebugAssert(pa != NULL);

    return pa->usage;
}

void ZetaPoolAllocator_DeployAllocator(void* pa_, Zeta_Allocator* dst) {
    ZetaPoolAllocator* pa = (ZetaPoolAllocator*)pa_;
    ZETA_DebugAssert(pa != NULL);

    dst->context = pa;

    dst->GetAlign = ZetaPoolAllocator_GetAlign;

    dst->Query = ZetaPoolAllocator_Query;

    dst->Allocate = ZetaPoolAllocator_Allocate;

    dst->Deallocate = ZetaPoolAllocator_Deallocate;
}
