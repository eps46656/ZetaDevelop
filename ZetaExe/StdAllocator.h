#include <unordered_map>

#include "../Zeta/Allocator.h"
#include "../Zeta/DebugHashMap.h"

struct StdAllocator {
    std::unordered_map<unsigned long long, unsigned long long> records;
    size_t usage;

    size_t buffered_size_{ 0 };
    size_t max_buffered_ptrs_num_{ 0 };
    std::vector<void*> buffered_ptrs_;
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

#if ZETA_IsDebug
    bool b{ sa->records.insert({ ZETA_PtrToAddr(ptr), size }).second };
    ZETA_DebugAssert(b);
    sa->usage += size;
#endif

    return ptr;
}

void StdAllocator_Deallocate(void* sa_, void* ptr) {
    StdAllocator* sa = (StdAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    if (ptr == NULL) { return; }

#if ZETA_IsDebug
    auto iter{ sa->records.find(ZETA_PtrToAddr(ptr)) };
    ZETA_DebugAssert(iter != sa->records.end());
    sa->usage -= iter->second;
    sa->records.erase(iter);
#endif

    free(ptr);
}

void StdAllocator_DeployAllocator(void* sa_, Zeta_Allocator* dst) {
    StdAllocator* sa = (StdAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    dst->context = sa;

    dst->GetAlign = StdAllocator_GetAlign;

    dst->Query = StdAllocator_Query;

    dst->Allocate = StdAllocator_Allocate;

    dst->Deallocate = StdAllocator_Deallocate;
}
