#include <deque>
#include <iostream>
#include <random>

#include "../Zeta/CircularVector.h"
#include "../Zeta/SegVector.h"
#include "StdAllocator.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef unsigned int val_t;

#define CURSOR_WIDTH ((size_t)(sizeof(void*) * 8))

#define CAPACITY (256 * 1024 * 1024)

StdAllocator node_allocator_;
Zeta_Allocator node_allocator;

StdAllocator seg_allocator_;
Zeta_Allocator seg_allocator;

Zeta_CircularVector cv;
Zeta_SegVector sv;

Zeta_SeqContainer seq_cntr;
Zeta_CursorOperator seq_cntr_cursor_opr;

void (*CheckCursor)(void* sc, void const* cursor);

std::deque<val_t> dd;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void SetVal(void* dst_, val_t val) { *(val_t*)dst_ = val; }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void InitCV() {
    cv.width = sizeof(val_t);
    cv.stride = sizeof(val_t);
    cv.offset = 0;
    cv.size = 0;
    cv.capacity = CAPACITY;
    cv.data = std::malloc(sizeof(val_t) * CAPACITY);

    Zeta_CircularVector_DeploySeqContainer(&cv, &seq_cntr);
    Zeta_CircularVector_Cursor_DeployCursorOperator(&cv, &seq_cntr_cursor_opr);

    CheckCursor = Zeta_CircularVector_Cursor_Check;

    ZETA_DebugAssert(seq_cntr_cursor_opr.cursor_width <= CURSOR_WIDTH);
}

void InitSV() {
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

void InitSeqCntr() { InitSV(); }

val_t* SC_Access(size_t idx) {
    byte_t cursor[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    void* ele = seq_cntr.Access(seq_cntr.context, &cursor, idx);

    ZETA_DebugAssert(
        ele == seq_cntr_cursor_opr.Refer(seq_cntr_cursor_opr.context, &cursor));

    return (val_t*)ele;
}

val_t* SC_PushL(val_t val) {
    byte_t pos_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    void* ele = seq_cntr.PushL(seq_cntr.context, &pos_cursor);

    void* re_ele =
        seq_cntr_cursor_opr.Refer(seq_cntr_cursor_opr.context, pos_cursor);

    ZETA_DebugAssert(ele == re_ele);

    SetVal(ele, val);

    byte_t re_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    seq_cntr.Access(seq_cntr.context, &re_cursor, 0);

    ZETA_DebugAssert(seq_cntr_cursor_opr.IsEqual(seq_cntr_cursor_opr.context,
                                                 pos_cursor, re_cursor));

    seq_cntr.PeekL(seq_cntr.context, &re_cursor);

    ZETA_DebugAssert(seq_cntr_cursor_opr.IsEqual(seq_cntr_cursor_opr.context,
                                                 pos_cursor, re_cursor));

    return (val_t*)ele;
}

val_t* SC_PushR(val_t val) {
    byte_t pos_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    void* ele = seq_cntr.PushR(seq_cntr.context, &pos_cursor);

    void* re_ele =
        seq_cntr_cursor_opr.Refer(seq_cntr_cursor_opr.context, pos_cursor);

    ZETA_DebugAssert(ele == re_ele);

    SetVal(ele, val);

    byte_t re_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    seq_cntr.Access(seq_cntr.context, &re_cursor,
                    seq_cntr.GetSize(seq_cntr.context) - 1);

    ZETA_DebugAssert(seq_cntr_cursor_opr.IsEqual(seq_cntr_cursor_opr.context,
                                                 pos_cursor, re_cursor));

    seq_cntr.PeekR(seq_cntr.context, &re_cursor);

    ZETA_DebugAssert(seq_cntr_cursor_opr.IsEqual(seq_cntr_cursor_opr.context,
                                                 pos_cursor, re_cursor));

    return (val_t*)ele;
}

void SC_Insert(size_t idx, val_t val) {
    byte_t pos_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    seq_cntr.Access(seq_cntr.context, &pos_cursor, idx);

    void* ele = seq_cntr.Insert(seq_cntr.context, &pos_cursor);

    void* re_ele =
        seq_cntr_cursor_opr.Refer(seq_cntr_cursor_opr.context, &pos_cursor);

    ZETA_DebugAssert(ele == re_ele);

    SetVal(ele, val);

    byte_t re_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    seq_cntr.Access(seq_cntr.context, &re_cursor, idx);

    ZETA_DebugAssert(seq_cntr_cursor_opr.IsEqual(seq_cntr_cursor_opr.context,
                                                 &pos_cursor, &re_cursor));
}

void SC_Erase(size_t idx) {
    byte_t pos_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    seq_cntr.Access(seq_cntr.context, &pos_cursor, idx);

    seq_cntr.Erase(seq_cntr.context, &pos_cursor);

    byte_t re_cursor[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    seq_cntr.Access(seq_cntr.context, &re_cursor, idx);

    ZETA_DebugAssert(seq_cntr_cursor_opr.IsEqual(seq_cntr_cursor_opr.context,
                                                 &pos_cursor, &re_cursor));
}

void SC_CheckIterator(size_t idx_a, size_t idx_b) {
    byte_t cursor_a[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    byte_t cursor_b[CURSOR_WIDTH]
        __attribute__((aligned(alignof(max_align_t))));

    size_t diff = idx_b - idx_a;

    seq_cntr.Access(seq_cntr.context, &cursor_a, idx_a);
    seq_cntr.Access(seq_cntr.context, &cursor_b, idx_b);
    seq_cntr_cursor_opr.AdvanceR(seq_cntr_cursor_opr.context, &cursor_a, diff);

    ZETA_DebugAssert(seq_cntr_cursor_opr.IsEqual(seq_cntr_cursor_opr.context,
                                                 &cursor_a, &cursor_b));

    //

    seq_cntr.Access(seq_cntr.context, &cursor_a, idx_a);
    seq_cntr.Access(seq_cntr.context, &cursor_b, idx_b);
    seq_cntr_cursor_opr.AdvanceL(seq_cntr_cursor_opr.context, &cursor_b, diff);

    ZETA_DebugAssert(seq_cntr_cursor_opr.IsEqual(seq_cntr_cursor_opr.context,
                                                 &cursor_a, &cursor_b));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

val_t* DD_Access(size_t idx) { return &dd[idx]; }

val_t* DD_PushL(val_t val) {
    dd.push_front(val);
    return &dd.front();
}

val_t* DD_PushR(val_t val) {
    dd.push_back(val);
    return &dd.back();
}

val_t* DD_Insert(size_t idx, val_t val) {
    auto iter = dd.begin() + idx;
    iter = dd.insert(iter, val);
    return &*iter;
}

void DD_Erase(size_t idx) { dd.erase(dd.begin() + idx); }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CheckCompare() {
    ZETA_DebugAssert(dd.size() == seq_cntr.GetSize(seq_cntr.context));

    for (size_t i{ 0 }; i < dd.size(); ++i) {
        ZETA_DebugAssert(dd[i] == *SC_Access(i));
    };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void main1() {
    unsigned int seed = time(NULL);

    ZETA_PrintVar("%d", seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> size_generator{ 0,
                                                          ZETA_maxof(size_t) };
    std::uniform_int_distribution<size_t> val_generator{ 0, ZETA_maxof(val_t) };

    InitSeqCntr();

    CheckCompare();

    for (int i = 0; i < 1000; ++i) {
        size_t idx = size_generator(en) % (dd.size() + 1);
        size_t val = val_generator(en);

        DD_Insert(idx, val);
        SC_Insert(idx, val);

        CheckCompare();
    }

    for (int _ = 0; _ < 100; ++_) {
        ZETA_PrintPos;

        for (int i = 0; i < 1000; ++i) {
            size_t idx = size_generator(en) % (dd.size() + 1);
            size_t val = val_generator(en);

            DD_Insert(idx, val);
            SC_Insert(idx, val);

            CheckCompare();
        }

        ZETA_PrintPos;

        for (int i = 0; i < 1000; ++i) {
            size_t val = val_generator(en);

            if (size_generator(en) % 2 == 0) {
                DD_PushL(val);
                SC_PushL(val);
            } else {
                DD_PushR(val);
                SC_PushR(val);
            }

            CheckCompare();
        }

        ZETA_PrintPos;

        for (int i = 0; i < 2000; ++i) {
            size_t idx_a = size_generator(en) % (dd.size() + 2);
            size_t idx_b = size_generator(en) % (dd.size() + 2);

            SC_CheckIterator(std::min(idx_a, idx_b) - 1,
                             std::max(idx_a, idx_b) - 1);
        }

        ZETA_PrintPos;

        for (int i = 0; i < 2000 && 0 < dd.size(); ++i) {
            size_t idx = size_generator(en) % dd.size();

            DD_Erase(idx);
            SC_Erase(idx);

            CheckCompare();
        }

        ZETA_PrintPos;
    }
}

int main() {
    main1();
    std::cout << "ok\n";
}
