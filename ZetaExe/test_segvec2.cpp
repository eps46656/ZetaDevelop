#include <deque>
#include <map>
#include <random>

#include "../Zeta/DebugDeque.h"
#include "../Zeta/SegVector.h"
#include "../Zeta/utils.h"
#include "MemAllocatorCheck.h"
#include "StdAllocator.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef unsigned int val_t;

#define CURSOR_WIDTH ((size_t)(sizeof(void*) * 8))

StdAllocator node_allocator_;
Zeta_Allocator node_allocator;

StdAllocator seg_allocator_;
Zeta_Allocator seg_allocator;

std::deque<val_t> dd;
Zeta_SegVector sv;

Zeta_SeqContainer seq_cntr;
Zeta_CursorOperator seq_cntr_cursor_opr;

Zeta_DebugTreeMap node_tm;
Zeta_DebugTreeMap seg_tm;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void SetupDebugDeque() {}

void SetupSegVector() {
    StdAllocator_ToAllocator(&node_allocator_, &node_allocator);
    StdAllocator_ToAllocator(&seg_allocator_, &seg_allocator);

    sv.width = sizeof(val_t);
    sv.seg_capacity = 32;
    sv.node_allocator = &node_allocator;
    sv.seg_allocator = &seg_allocator;

    Zeta_SegVector_Init(&sv);
    Zeta_SegVector_DeploySeqContainer(&sv, &seq_cntr);

    Zeta_SegVector_Cursor_DeployCursorOperator(&sv, &seq_cntr_cursor_opr);
}

bool_t SV_LowerBound_(void* val, void* ele) {
    return *(val_t*)val <= *(val_t*)ele;
}

bool_t SV_UpperBound_(void* val, void* ele) {
    return *(val_t*)val < *(val_t*)ele;
}

size_t SV_LowerBound(val_t val) {
    byte_t cursor[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_FindFirst(&sv, &cursor, &val, SV_LowerBound_);

    Zeta_SegVector_Cursor_Check(&sv, &cursor);

    byte_t beg_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_PeekL(&sv, &beg_cursor);

    return Zeta_SegVector_Cursor_Differ(&sv, &cursor, &beg_cursor);
}

size_t SV_UpperBound(val_t val) {
    byte_t cursor[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_FindFirst(&sv, &cursor, &val, SV_UpperBound_);

    Zeta_SegVector_Cursor_Check(&sv, &cursor);

    byte_t beg_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_PeekL(&sv, &beg_cursor);

    return Zeta_SegVector_Cursor_Differ(&sv, &cursor, &beg_cursor);
}

size_t SV_Insert(val_t val) {
    byte_t cursor[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_FindFirst(&sv, &cursor, &val, SV_UpperBound_);

    Zeta_SegVector_Cursor_Check(&sv, &cursor);

    byte_t beg_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_PeekL(&sv, &beg_cursor);

    size_t idx = Zeta_SegVector_Cursor_Differ(&sv, &cursor, &beg_cursor);

    *(val_t*)Zeta_SegVector_Insert(&sv, &cursor) = val;

    return idx;
}

void SV_Erase(size_t idx) {
    byte_t cursor[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_Access(&sv, &cursor, idx);

    Zeta_SegVector_Erase(&sv, &cursor);
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
        ZETA_DebugAssert(dd[i] == *(val_t*)Zeta_SegVector_Access(&sv, NULL, i));
    }

    std::map<size_t, size_t>* node_tree_map =
        (std::map<size_t, size_t>*)node_tm.tree_map;

    std::map<size_t, size_t>* seg_tree_map =
        (std::map<size_t, size_t>*)seg_tm.tree_map;

    node_tree_map->clear();
    seg_tree_map->clear();

    Zeta_SegVector_Check(&sv, &node_tm, &seg_tm);

    CheckFullContains(node_allocator_.records, *node_tree_map);
    CheckFullContains(seg_allocator_.records, *seg_tree_map);
}

void main1() {
    unsigned int seed = time(NULL);

    ZETA_PrintVar("%d", seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> size_generator{ 0,
                                                          ZETA_maxof(size_t) };
    std::uniform_int_distribution<size_t> val_generator{ 0, 16 };

    StdAllocator_ToAllocator(&node_allocator_, &node_allocator);
    StdAllocator_ToAllocator(&seg_allocator_, &seg_allocator);

    SetupSegVector();
    SetupDebugDeque();

    Zeta_DebugTreeMap_Create(&node_tm);
    Zeta_DebugTreeMap_Create(&seg_tm);

    Check();

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
