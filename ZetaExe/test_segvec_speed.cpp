#include <chrono>
#include <iostream>
#include <map>
#include <random>

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

Zeta_DebugDeque dd_;
Zeta_SegVector sv_;

Zeta_Vector dd;
Zeta_Vector sv;

Zeta_DebugTreeMap node_tm;
Zeta_DebugTreeMap seg_tm;

void SetupDebugDeque() {
    dd_.width = WIDTH;
    Zeta_DebugDeque_Create(&dd_);
    Zeta_DebugDeque_ToVector(&dd_, &dd);
}

void SetupSegVector() {
    sv_.width = WIDTH;
    sv_.seg_capacity = 32;
    sv_.node_allocator = &node_allocator;
    sv_.seg_allocator = &seg_allocator;
    Zeta_SegVector_Init(&sv_);
    Zeta_SegVector_ToVector(&sv_, &sv);
};

void Check() {
    size_t size = dd.GetSize(dd.context);
    ZETA_DebugAssert(sv.GetSize(sv.context) == size);

    for (size_t i = 0; i < size; ++i) {
        ZETA_DebugAssert(
            CmpVal(dd.Access(dd.context, i), sv.Access(sv.context, i)));
    }

    std::map<size_t, size_t>* node_tree_map =
        (std::map<size_t, size_t>*)node_tm.tree_map;

    std::map<size_t, size_t>* seg_tree_map =
        (std::map<size_t, size_t>*)seg_tm.tree_map;

    node_tree_map->clear();
    seg_tree_map->clear();

    Zeta_SegVector_Check(&sv_, &node_tm, &seg_tm);

    CheckFullContains(node_allocator_.records, *node_tree_map);
    CheckFullContains(seg_allocator_.records, *seg_tree_map);
}

void main1() {
    unsigned int seed = time(NULL);

    ZETA_PrintVar("%d", seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> size_generator{ 0, ZETA_GetRangeMax(
                                                                 size_t) };
    std::uniform_int_distribution<size_t> val_generator{ 0, ZETA_GetRangeMax(
                                                                val_t) };

    StdAllocator_ToAllocator(&node_allocator_, &node_allocator);
    StdAllocator_ToAllocator(&seg_allocator_, &seg_allocator);

    SetupSegVector();
    SetupDebugDeque();

    // Zeta_DebugTreeMap_Create(&node_tm);
    // Zeta_DebugTreeMap_Create(&seg_tm);

    // Check();

#define V dd

    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();

    for (int i = 0; i < 1000; ++i) {
        size_t idx = size_generator(en) % (V.GetSize(V.context) + 1);
        size_t val = val_generator(en);

        SetVal(V.Insert(V.context, idx), val);
    }

    for (int _ = 0; _ < 100; ++_) {
        for (int i = 0; i < 200000; ++i) {
            size_t idx = size_generator(en) % (V.GetSize(V.context) + 1);
            size_t val = val_generator(en);

            SetVal(V.Insert(V.context, idx), val);
        }

        for (int i = 0; i < 200000; ++i) {
            size_t idx = size_generator(en) % V.GetSize(V.context);

            V.Erase(V.context, idx);
        }
    }

    {
        int size = V.GetSize(V.context);

        val_t sum = 0;

        for (int i = 0; i < size; ++i) {
            sum += *(val_t*)(V.Access(V.context, i));
        }

        ZETA_PrintVar("%lld", sum);
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
