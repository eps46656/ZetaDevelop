#pragma once

#include <allocator.h>
#include <debug_hash_table.h>
#include <debugger.h>
#include <mem_check_utils.h>

#include <unordered_map>

struct StdAllocator {
    Zeta_MemRecorder* mem_recorder;

    size_t usage;

    size_t buffered_size_{ 0 };
    size_t max_buffered_ptrs_num_{ 0 };
    std::vector<void*> buffered_ptrs_;

    StdAllocator();
    ~StdAllocator();
};

StdAllocator::StdAllocator() { this->mem_recorder = Zeta_MemRecorder_Create(); }

StdAllocator::~StdAllocator() { Zeta_MemRecorder_Destroy(this->mem_recorder); }

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

    void* ptr = std::malloc(size);

#if ZETA_IsDebug
    Zeta_MemRecorder_Record(sa->mem_recorder, ptr, size);
    sa->usage += size;
#endif

    return ptr;
}

void StdAllocator_Deallocate(void* sa_, void* ptr) {
    StdAllocator* sa = (StdAllocator*)sa_;
    ZETA_DebugAssert(sa != NULL);

    if (ptr == NULL) { return; }

#if ZETA_IsDebug
    ZETA_DebugAssert(Zeta_MemRecorder_Unrecord(sa->mem_recorder, ptr));
#endif

    std::free(ptr);
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
