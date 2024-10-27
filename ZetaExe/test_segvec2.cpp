#include <deque>
#include <map>
#include <random>

#include "../Zeta/SegVector.h"
#include "../Zeta/utils.h"
#include "std_allocator.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef unsigned val_t;

#define CURSOR_WIDTH ((size_t)(sizeof(void*) * 8))

StdAllocator node_allocator_;
Zeta_Allocator node_allocator;

StdAllocator seg_allocator_;
Zeta_Allocator seg_allocator;

std::deque<val_t> dd;
Zeta_SegVector sv;

Zeta_SeqCntr seq_cntr;
Zeta_CursorOperator seq_cntr_cursor_opr;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void SetupSegVector() {
    StdAllocator_DeployAllocator(&node_allocator_, &node_allocator);
    StdAllocator_DeployAllocator(&seg_allocator_, &seg_allocator);

    sv.width = sizeof(val_t);
    sv.stride = sizeof(val_t);
    sv.seg_capacity = 32;
    sv.node_allocator = &node_allocator;
    sv.seg_allocator = &seg_allocator;

    ZETA_PrintPos;

    Zeta_SegVector_Init(&sv);

    ZETA_PrintPos;

    Zeta_SegVector_DeploySeqCntr(&sv, &seq_cntr);
}

bool_t SV_LowerBound_(void* val, void* ele) {
    return *(val_t*)val <= *(val_t*)ele;
}

bool_t SV_UpperBound_(void* val, void* ele) {
    return *(val_t*)val < *(val_t*)ele;
}

size_t SV_LowerBound(val_t val) {
    byte_t cursor[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_FindFirst(&sv, &cursor, NULL, &val, SV_LowerBound_);

    Zeta_SegVector_Cursor_Check(&sv, &cursor);

    byte_t beg_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_PeekL(&sv, &beg_cursor, NULL);

    return Zeta_SegVector_Cursor_GetDist(&sv, &cursor, &beg_cursor);
}

size_t SV_UpperBound(val_t val) {
    byte_t cursor[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_FindFirst(&sv, &cursor, NULL, &val, SV_UpperBound_);

    Zeta_SegVector_Cursor_Check(&sv, &cursor);

    byte_t beg_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_PeekL(&sv, &beg_cursor, NULL);

    return Zeta_SegVector_Cursor_GetDist(&sv, &cursor, &beg_cursor);
}

size_t SV_Insert(val_t val) {
    byte_t cursor[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_FindFirst(&sv, &cursor, NULL, &val, SV_UpperBound_);

    Zeta_SegVector_Cursor_Check(&sv, &cursor);

    byte_t beg_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_PeekL(&sv, &beg_cursor, NULL);

    size_t idx = Zeta_SegVector_Cursor_GetDist(&sv, &cursor, &beg_cursor);

    *(val_t*)Zeta_SegVector_Insert(&sv, &cursor, 1) = val;

    return idx;
}

void SV_Erase(size_t idx) {
    byte_t cursor[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_Access(&sv, &cursor, NULL, idx);

    Zeta_SegVector_Erase(&sv, &cursor, 1);
}

size_t DD_LowerBound(val_t val) {
    return (size_t)(std::lower_bound(dd.begin(), dd.end(), val) - dd.begin());
}

size_t DD_UpperBound(val_t val) {
    return (size_t)(std::upper_bound(dd.begin(), dd.end(), val) - dd.begin());
}

size_t DD_Insert(val_t val) {
    size_t idx = DD_UpperBound(val);
    dd.insert(dd.begin() + idx, val);
    return idx;
}

void DD_Erase(size_t idx) { dd.erase(dd.begin() + idx); }

void Check() {
    size_t size = dd.size();
    ZETA_DebugAssert(Zeta_SegVector_GetSize(&sv) == size);

    for (size_t i = 0; i < size; ++i) {
        ZETA_DebugAssert(dd[i] ==
                         *(val_t*)Zeta_SegVector_Access(&sv, NULL, NULL, i));
    }

    using record_t = std::unordered_map<unsigned long long, unsigned long long>;

    Zeta_DebugHashTable node_hm;
    Zeta_DebugHashTable seg_hm;

    Zeta_DebugHashTable_Init(&node_hm);
    Zeta_DebugHashTable_Init(&seg_hm);

    Zeta_SegVector_Check(&sv, &node_hm, &seg_hm);

    using record_t = std::unordered_map<unsigned long long, unsigned long long>;

    CheckRecords(node_allocator_.records, *(record_t*)node_hm.hash_map);

    CheckRecords(seg_allocator_.records, *(record_t*)seg_hm.hash_map);
}

void main1() {
    unsigned seed = time(NULL);

    ZETA_PrintVar(seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> size_generator{ 0, ZETA_RangeMaxOf(
                                                                 size_t) };
    std::uniform_int_distribution<size_t> val_generator{ 0, 16 };

    StdAllocator_DeployAllocator(&node_allocator_, &node_allocator);
    StdAllocator_DeployAllocator(&seg_allocator_, &seg_allocator);

    SetupSegVector();

    Check();

    ZETA_PrintPos;

    for (int i = 0; i < 1000; ++i) {
        size_t val = val_generator(en);

        size_t sv_idx = SV_Insert(val);
        size_t dd_idx = DD_Insert(val);

        ZETA_DebugAssert(sv_idx == dd_idx);

        Check();
    }

    for (int _ = 0; _ < 20; ++_) {
        for (int i = 0; i < 2000; ++i) {
            size_t val = val_generator(en);

            size_t sv_idx = SV_Insert(val);
            size_t dd_idx = DD_Insert(val);

            ZETA_DebugAssert(sv_idx == dd_idx);

            Check();
        }

        for (int i = 0; i < 2000; ++i) {
            size_t val = val_generator(en);

            ZETA_DebugAssert(DD_LowerBound(val) == SV_LowerBound(val));
            ZETA_DebugAssert(DD_UpperBound(val) == SV_UpperBound(val));

            Check();
        }

        for (int i = 0; i < 2000; ++i) {
            size_t idx = size_generator(en) % dd.size();

            DD_Erase(idx);
            SV_Erase(idx);

            Check();
        }
    }
}

int main() {
    main1();
    printf("ok\n");
    return 0;
}
