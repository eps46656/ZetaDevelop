#include <chrono>
#include <deque>
#include <iostream>
#include <map>
#include <random>

#include "../Zeta/SegVector.h"
#include "../Zeta/debug_deque.h"
#include "../Zeta/utils.h"
#include "std_allocator.h"

typedef unsigned val_t;

#define CURSOR_WIDTH ((size_t)(sizeof(void*) * 8))

StdAllocator node_allocator_;
Zeta_Allocator node_allocator;

StdAllocator seg_allocator_;
Zeta_Allocator seg_allocator;

std::deque<val_t> dd;
Zeta_SegVector sv;

Zeta_DebugHashTable node_hm;
Zeta_DebugHashTable seg_hm;

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void SV_Init() {
    StdAllocator_DeployAllocator(&node_allocator_, &node_allocator);
    StdAllocator_DeployAllocator(&seg_allocator_, &seg_allocator);

    sv.width = sizeof(val_t);
    sv.seg_capacity = 32;
    sv.node_allocator = &node_allocator;
    sv.seg_allocator = &seg_allocator;

    Zeta_SegVector_Init(&sv);
}

size_t SV_GetSize() { return Zeta_SegVector_GetSize(&sv); }

val_t* SV_Access(size_t idx) {
    return (val_t*)Zeta_SegVector_Access(&sv, NULL, idx);
}

void SV_Insert(size_t idx, val_t val) {
    byte_t cursor[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_Access(&sv, &cursor, idx);

    *(val_t*)Zeta_SegVector_Insert(&sv, &cursor) = val;
}

void SV_Erase(size_t idx) {
    byte_t cursor[CURSOR_WIDTH] __attribute__((aligned(alignof(max_align_t))));

    Zeta_SegVector_Access(&sv, &cursor, idx);

    Zeta_SegVector_Erase(&sv, &cursor);
}

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

size_t DD_GetSize() { return dd.size(); }

val_t* DD_Access(size_t idx) { return &dd[idx]; }

void DD_Insert(size_t idx, val_t val) { dd.insert(dd.begin() + idx, val); }

void DD_Erase(size_t idx) { dd.erase(dd.begin() + idx); }

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

void main1() {
    unsigned seed = time(NULL);

    ZETA_PrintVar(seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> size_generator{ 0, ZETA_RangeMaxOf(
                                                                 size_t) };
    std::uniform_int_distribution<size_t> val_generator{ 0, ZETA_RangeMaxOf(
                                                                val_t) };

    StdAllocator_DeployAllocator(&node_allocator_, &node_allocator);
    StdAllocator_DeployAllocator(&seg_allocator_, &seg_allocator);

    SV_Init();

    size_t (*GetSize)();
    val_t* (*Access)(size_t idx);
    void (*Insert)(size_t idx, val_t val);
    void (*Erase)(size_t idx);

    bool_t is_sv = TRUE;

    if (is_sv) {
        GetSize = SV_GetSize;
        Access = SV_Access;
        Insert = SV_Insert;
        Erase = SV_Erase;
    } else {
        GetSize = DD_GetSize;
        Access = DD_Access;
        Insert = DD_Insert;
        Erase = DD_Erase;
    }

    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();

    for (int i = 0; i < 1000; ++i) {
        size_t idx = size_generator(en) % (GetSize() + 1);
        size_t val = val_generator(en);

        Insert(idx, val);
    }

    for (int _ = 0; _ < 100; ++_) {
        for (int i = 0; i < 20000; ++i) {
            size_t idx = size_generator(en) % (GetSize() + 1);
            size_t val = val_generator(en);

            Insert(idx, val);
        }

        for (int i = 0; i < 20000; ++i) {
            size_t idx = size_generator(en) % GetSize();

            Erase(idx);
        }
    }

    {
        size_t size = GetSize();

        val_t sum = 0;

        for (int i = 0; i < size; ++i) { sum += *Access(i); }

        ZETA_PrintVar(sum);
    }

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();

    std::cout << "Time difference = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       begin)
                     .count()
              << "[ms]\n";
}

int main() {
    main1();
    printf("ok\n");
    return 0;
}
