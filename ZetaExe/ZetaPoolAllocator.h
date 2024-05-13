
#include <unordered_map>
#include <vector>

#include "../Zeta/Allocator.h"

struct ZetaPoolAllocator {
    size_t size;

    size_t max_buffered_ptrs_num;
    std::vector<void*> buffered_ptrs;

    std::unordered_map<unsigned long long, unsigned long long> records;
    size_t usage;
};

size_t ZetaPoolAllocator_GetAlign(void* sa_) {
    ZETA_Unused(sa_);
    return alignof(max_align_t);
}

size_t ZetaPoolAllocator_Query(void* sa_, size_t size) {
    ZetaPoolAllocator* sa = (ZetaPoolAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    if (size == 0) { return 0; }
    if (size <= sa->size) { return sa->size; }
    return 0;
}

void* ZetaPoolAllocator_Allocate(void* sa_, size_t size) {
    ZetaPoolAllocator* sa = (ZetaPoolAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    if (size == 0 || sa->size < size) { return NULL; }

    void* ptr;

    if (!sa->buffered_ptrs.empty()) {
        ptr = sa->buffered_ptrs.back();
        sa->buffered_ptrs.pop_back();
    } else {
        ptr = malloc(sa->size);
    }

#if ZETA_IsDebug
    bool b{ sa->records.insert({ ZETA_GetAddrFromPtr(ptr), sa->size }).second };
    ZETA_DebugAssert(b);
    sa->usage += sa->size;
#endif

    return ptr;
}

void ZetaPoolAllocator_Deallocate(void* sa_, void* ptr) {
    ZetaPoolAllocator* sa = (ZetaPoolAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    if (ptr == NULL) { return; }

#if ZETA_IsDebug
    auto iter{ sa->records.find(ZETA_GetAddrFromPtr(ptr)) };
    ZETA_DebugAssert(iter != sa->records.end());
    sa->usage -= iter->second;
    sa->records.erase(iter);
#endif

    if (sa->buffered_ptrs.size() < sa->max_buffered_ptrs_num) {
        sa->buffered_ptrs.push_back(ptr);
    } else {
        free(ptr);
    }
}

void ZetaPoolAllocator_DeployAllocator(void* sa_, Zeta_Allocator* dst) {
    ZetaPoolAllocator* sa = (ZetaPoolAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    dst->context = sa;

    dst->GetAlign = ZetaPoolAllocator_GetAlign;

    dst->Query = ZetaPoolAllocator_Query;

    dst->Allocate = ZetaPoolAllocator_Allocate;

    dst->Deallocate = ZetaPoolAllocator_Deallocate;
}
