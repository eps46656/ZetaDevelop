
#include <unordered_set>

struct StdAllocator {
    std::unordered_map<void*, size_t> ptr_size;
};

size_t StdAllocator_Query(void* sa_, size_t size) {
    ZETA_Unused(sa_);
    return 0 < size ? size : 0;
}

void* StdAllocator_Allocate(void* sa_, size_t size) {
    StdAllocator* sa = (StdAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    if (size == 0) { return NULL; }

    void* ptr = malloc(size);
    sa->ptr_size.insert({ ptr, size });

    return ptr;
}

void StdAllocator_Deallocate(void* sa_, void* ptr) {
    StdAllocator* sa = (StdAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    if (ptr == NULL) { return; }

    bool_t b = sa->ptr_size.erase(ptr) != 0;
    ZETA_DebugAssert(b);

    free(ptr);
}

void StdAllocator_ToAllocator(void* sa_, Zeta_Allocator* dst) {
    StdAllocator* sa = (StdAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    dst->context = sa;

    dst->Query = StdAllocator_Query;
    dst->Allocate = StdAllocator_Allocate;
    dst->Deallocate = StdAllocator_Deallocate;
}
