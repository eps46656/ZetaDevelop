#include <chrono>
#include <deque>
#include <iostream>
#include <map>
#include <random>
#include <set>

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

std::multiset<val_t> ms;
std::deque<val_t> dd;
Zeta_SegVector sv;

Zeta_SeqCntr seq_cntr;

void SV_Init() {
    StdAllocator_DeployAllocator(&node_allocator_, &node_allocator);
    StdAllocator_DeployAllocator(&seg_allocator_, &seg_allocator);

    sv.width = sizeof(val_t);
    sv.width = sizeof(val_t);
    sv.seg_capacity = 32;
    sv.node_allocator = &node_allocator;
    sv.seg_allocator = &seg_allocator;

    Zeta_SegVector_Init(&sv);
    Zeta_SegVector_DeploySeqCntr(&sv, &seq_cntr);
}

bool_t SV_LowerBound_(void* val, void* ele) {
    return *(val_t*)val <= *(val_t*)ele;
}

bool_t SV_UpperBound_(void* val, void* ele) {
    return *(val_t*)val < *(val_t*)ele;
}

val_t SV_LowerBound(val_t val) {
    val_t* ele =
        (val_t*)Zeta_SegVector_FindFirst(&sv, NULL, NULL, &val, SV_LowerBound_);
    return ele == NULL ? ZETA_RangeMaxOf(val_t) : *ele;
}

val_t SV_UpperBound(val_t val) {
    val_t* ele =
        (val_t*)Zeta_SegVector_FindFirst(&sv, NULL, NULL, &val, SV_UpperBound_);
    return ele == NULL ? ZETA_RangeMaxOf(val_t) : *ele;
}

void SV_Insert(val_t val) {
    Zeta_Cursor cursor;

    Zeta_SegVector_FindFirst(&sv, &cursor, NULL, &val, SV_UpperBound_);

    *(val_t*)Zeta_SegVector_Insert(&sv, &cursor, 1) = val;
}

void SV_Erase(val_t val) {
    Zeta_Cursor cursor;

    val_t* ele = (val_t*)Zeta_SegVector_FindFirst(&sv, &cursor, NULL, &val,
                                                  SV_LowerBound_);

    if (ele != NULL && *ele == val) { Zeta_SegVector_Erase(&sv, &cursor, 1); }
}

val_t MS_LowerBound(val_t val) {
    auto iter = ms.lower_bound(val);
    return iter == ms.end() ? ZETA_RangeMaxOf(val_t) : *iter;
}

val_t MS_UpperBound(val_t val) {
    auto iter = ms.upper_bound(val);
    return iter == ms.end() ? ZETA_RangeMaxOf(val_t) : *iter;
}

void MS_Insert(val_t val) { ms.insert(val); }

void MS_Erase(val_t val) {
    auto iter{ ms.find(val) };
    if (iter != ms.end()) { ms.erase(iter); }
}

void main1() {
    unsigned seed = time(NULL);
    // unsigned seed = 1714030320;

    ZETA_PrintVar(seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> size_generator{ 0, ZETA_RangeMaxOf(
                                                                 size_t) };
    std::uniform_int_distribution<size_t> val_generator{ 0, ZETA_RangeMaxOf(
                                                                size_t) };

    StdAllocator_DeployAllocator(&node_allocator_, &node_allocator);
    StdAllocator_DeployAllocator(&seg_allocator_, &seg_allocator);

    SV_Init();

    val_t (*LowerBound)(val_t val);
    val_t (*UpperBound)(val_t val);
    void (*Insert)(val_t val);
    void (*Erase)(val_t val);

    bool_t is_sv{ FALSE };

    val_t dummy{ 0 };

    if (is_sv) {
        LowerBound = SV_LowerBound;
        UpperBound = SV_UpperBound;
        Insert = SV_Insert;
        Erase = SV_Erase;
    } else {
        LowerBound = MS_LowerBound;
        UpperBound = MS_UpperBound;
        Insert = MS_Insert;
        Erase = MS_Erase;
    }

    ZETA_Unused(LowerBound);
    ZETA_Unused(UpperBound);
    ZETA_Unused(Insert);
    ZETA_Unused(Erase);

    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();

    for (int i = 0; i < 10000; ++i) {
        size_t val = val_generator(en);
        Insert(val);
    }

    for (int _ = 0; _ < 20; ++_) {
        for (int i = 0; i < 200000; ++i) {
            size_t val = val_generator(en);

            Insert(val);
        }

        if (TRUE) {
            for (int i = 0; i < 200000; ++i) {
                size_t val = val_generator(en);
                dummy += UpperBound(val);
            }

            for (int i = 0; i < 200000; ++i) {
                size_t val = val_generator(en);
                dummy += LowerBound(val);
            }
        }

        if (TRUE) {
            for (int i = 0; i < 200000; ++i) {
                size_t val = val_generator(en);

                Erase(val);
            }
        }

        if (TRUE) {
            for (int i = 0; i < 200000; ++i) {
                size_t val = val_generator(en);
                dummy += LowerBound(val);
            }

            for (int i = 0; i < 200000; ++i) {
                size_t val = val_generator(en);
                dummy += UpperBound(val);
            }
        }
    }

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();

    std::cout << "Time difference = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       begin)
                     .count()
              << "[ms]\n";

    ZETA_PrintVar(dummy);
}

int main() {
    main1();
    printf("ok\n");
    return 0;
}
