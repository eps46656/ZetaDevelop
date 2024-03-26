#include <chrono>
#include <deque>
#include <iostream>
#include <map>
#include <random>
#include <set>

#include "../Zeta/DebugDeque.h"
#include "../Zeta/SegVector.h"
#include "../Zeta/utils.h"
#include "MemAllocatorCheck.h"
#include "StdAllocator.h"

typedef unsigned int val_t;

#define WIDTH sizeof(val_t)

void SetVal(void* dst, val_t x) { Zeta_MemCopy(dst, &x, WIDTH); }

void PrintVal(void* x) {
    unsigned int xi;
    Zeta_MemCopy(&xi, x, WIDTH);
    printf("%x\n", xi);
}

bool CmpVal(void* x_, void* y_) {
    char* x = (char*)x_;
    char* y = (char*)y_;

    for (size_t i = 0; i < WIDTH; ++i) {
        if (x[i] != y[i]) { return FALSE; }
    }

    return TRUE;
}

StdAllocator node_allocator_;
Zeta_Allocator node_allocator;

StdAllocator seg_allocator_;
Zeta_Allocator seg_allocator;

std::multiset<val_t> ms;
std::deque<val_t> dd;
Zeta_SegVector sv;

Zeta_DebugTreeMap node_tm;
Zeta_DebugTreeMap seg_tm;

void SetupDebugDeque() {}

void SetupSegVector() {
    sv.width = WIDTH;
    sv.seg_capacity = 32;
    sv.node_allocator = &node_allocator;
    sv.seg_allocator = &seg_allocator;
    Zeta_SegVector_Init(&sv);
}

bool_t SV_LowerBound_(void* val, void* ele) {
    return *(val_t*)val <= *(val_t*)ele;
}

bool_t SV_UpperBound_(void* val, void* ele) {
    return *(val_t*)val < *(val_t*)ele;
}

val_t SV_LowerBound(val_t val) {
    size_t idx;
    void* ele;

    Zeta_SegVector_FindFirst(&sv, &idx, &ele, &val, SV_LowerBound_);

    size_t size = Zeta_SegVector_GetSize(&sv);

    return idx == size ? ZETA_maxof(val_t) : *(val_t*)ele;
}

val_t SV_UpperBound(val_t val) {
    size_t idx;
    void* ele;

    Zeta_SegVector_FindFirst(&sv, &idx, &ele, &val, SV_UpperBound_);

    size_t size = Zeta_SegVector_GetSize(&sv);

    return idx == size ? ZETA_maxof(val_t) : *(val_t*)ele;
}

void SV_Insert(val_t val) {
    size_t idx;
    Zeta_SegVector_FindFirst(&sv, &idx, NULL, &val, SV_UpperBound_);
    *(val_t*)Zeta_SegVector_Insert(&sv, idx) = val;
}

void SV_Erase(val_t val) {
    size_t idx;

    Zeta_SegVector_FindFirst(&sv, &idx, NULL, &val, SV_LowerBound_);

    size_t size = Zeta_SegVector_GetSize(&sv);

    if (idx < size) { Zeta_SegVector_Erase(&sv, idx); }
}

val_t MS_LowerBound(val_t val) {
    auto iter = ms.lower_bound(val);
    return iter == ms.end() ? ZETA_maxof(val_t) : *iter;
}

val_t MS_UpperBound(val_t val) {
    auto iter = ms.upper_bound(val);
    return iter == ms.end() ? ZETA_maxof(val_t) : *iter;
}

void MS_Insert(val_t val) { ms.insert(val); }

void MS_Erase(val_t val) {
    auto iter{ ms.find(val) };
    if (iter != ms.end()) { ms.erase(iter); }
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
        for (int i = 0; i < 20000; ++i) {
            size_t val = val_generator(en);

            Insert(val);
        }

        for (int i = 0; i < 20000; ++i) {
            size_t val = val_generator(en);
            dummy += UpperBound(val);
        }

        for (int i = 0; i < 20000; ++i) {
            size_t val = val_generator(en);
            dummy += LowerBound(val);
        }

        if (FALSE) {
            for (int i = 0; i < 20000; ++i) {
                size_t val = val_generator(en);

                Erase(val);
            }
        }

        for (int i = 0; i < 20000; ++i) {
            size_t val = val_generator(en);
            dummy += LowerBound(val);
        }

        for (int i = 0; i < 20000; ++i) {
            size_t val = val_generator(en);
            dummy += UpperBound(val);
        }
    }

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();

    std::cout << "Time difference = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       begin)
                     .count()
              << "[ms]\n";

    ZETA_PrintVar("%u", dummy);
}

int main() {
    main1();
    printf("ok\n");
    return 0;
}
