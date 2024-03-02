
#include <map>

#include "../Zeta/define.h"

#define RECORDS TRUE

struct StdAllocator {
    std::map<size_t, size_t> records;
};

size_t StdAllocator_GetAlign(void* sa_) {
    ZETA_Unused(sa_);
    return alignof(max_align_t);
}

size_t StdAllocator_Query(void* sa_, size_t size) {
    ZETA_Unused(sa_);
    return 0 < size ? size : 0;
}

void* StdAllocator_Allocate(void* sa_, size_t size) {
    StdAllocator* sa = (StdAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    if (size == 0) { return NULL; }

    void* ptr = malloc(size);

#if RECORDS
    sa->records.insert({ ZETA_PTR_TO_UINT(ptr), size });
#endif

    return ptr;
}

void StdAllocator_Deallocate(void* sa_, void* ptr) {
    StdAllocator* sa = (StdAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    if (ptr == NULL) { return; }

#if RECORDS
    bool_t b = sa->records.erase(ZETA_PTR_TO_UINT(ptr)) != 0;
    ZETA_DebugAssert(b);
#endif

    free(ptr);
}

void StdAllocator_ToAllocator(void* sa_, Zeta_Allocator* dst) {
    StdAllocator* sa = (StdAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    dst->context = sa;
    dst->GetAlign = StdAllocator_GetAlign;
    dst->Query = StdAllocator_Query;
    dst->Allocate = StdAllocator_Allocate;
    dst->Deallocate = StdAllocator_Deallocate;
}
