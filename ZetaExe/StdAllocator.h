
#include <map>

#include "../Zeta/Allocator.h"

struct StdAllocator {
    std::unordered_map<unsigned long long, unsigned long long> records;
    size_t usage;

    size_t buffered_size_;
    size_t max_buffered_ptrs_num_;
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

    ZETA_PrintVar(size);

#if ZETA_IsDebug
    bool b{ sa->records.insert({ ZETA_GetAddrFromPtr(ptr), size }).second };
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
    auto iter{ sa->records.find(ZETA_GetAddrFromPtr(ptr)) };
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

void StdAllocator_CheckRecords(void* sa_, Zeta_DebugTreeMap const* records) {
    if (!ZETA_IsDebug) { return; }

    StdAllocator* sa = (StdAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    std::unordered_map<unsigned long long, unsigned long long>* m1 =
        &sa->records;

    std::map<unsigned long long, unsigned long long>* m2 =
        (std::map<unsigned long long, unsigned long long>*)records->tree_map;

    for (auto iter{ m1->begin() }, end{ m1->end() }; iter != end; ++iter) {
        auto iter_b{ m2->find(iter->first) };

        ZETA_DebugAssert(iter_b != m2->end());
        // leak: user misses allocated memory

        ZETA_DebugAssert(iter_b->second <= iter->second);
        // overflow: usee uses more memory than allocated
    }

    for (auto iter{ m2->begin() }, end{ m2->end() }; iter != end; ++iter) {
        auto iter_b{ m1->find(iter->first) };
        ZETA_DebugAssert(iter_b != m1->end());
        // hallucination: user use XXX memory

        ZETA_DebugAssert(iter->second <= iter_b->second);
        // overflow: user uses more memory than allocated
    }
}
