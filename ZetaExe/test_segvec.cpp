#include <deque>
#include <random>
#include <unordered_map>

#include "../Zeta/SegVector.h"
#include "../Zeta/utils.h"
#include "MemAllocatorCheck.h"
#include "StdAllocator.h"

typedef unsigned val_t;

#define CURSOR_WIDTH ((size_t)(sizeof(void*) * 8))

void SetVal(void* dst, val_t x) { Zeta_MemCopy(dst, &x, sizeof(val_t)); }

void PrintVal(void* x) {
    unsigned xi;
    Zeta_MemCopy(&xi, x, sizeof(val_t));
    printf("%x\n", xi);
}

bool_t CmpVal(void* x_, void* y_) {
    char* x = (char*)x_;
    char* y = (char*)y_;

    for (size_t i = 0; i < sizeof(val_t); ++i) {
        if (x[i] != y[i]) { return FALSE; }
    }

    return TRUE;
}

StdAllocator node_allocator_;
Zeta_Allocator node_allocator;

StdAllocator seg_allocator_;
Zeta_Allocator seg_allocator;

std::deque<val_t> dd;
Zeta_SegVector sv;

Zeta_DebugHashMap node_hm;
Zeta_DebugHashMap seg_hm;

void DD_Init() {}

size_t DD_GetSize() { return dd.size(); }

val_t DD_Access(size_t idx) { return dd[idx]; }

void DD_Insert(size_t idx, val_t val) { dd.insert(dd.begin() + idx, val); }

void DD_Erase(size_t idx) { dd.erase(dd.begin() + idx); }

void SV_Init() {
    sv.width = sizeof(val_t);
    sv.seg_capacity = 4;
    sv.node_allocator = &node_allocator;
    sv.seg_allocator = &seg_allocator;
    Zeta_SegVector_Init(&sv);
};

size_t SV_GetSize() { return Zeta_SegVector_GetSize(&sv); }

val_t SV_Access(size_t idx) {
    return *(val_t*)Zeta_SegVector_Access(&sv, NULL, idx);
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

void Check() {
    size_t size = DD_GetSize();
    ZETA_DebugAssert(SV_GetSize() == size);

    for (size_t i = 0; i < size; ++i) {
        ZETA_DebugAssert(DD_Access(i) == SV_Access(i));
    }

    std::unordered_map<unsigned long long, unsigned long long>* node_hash_map =
        (std::unordered_map<unsigned long long, unsigned long long>*)
            node_hm.hash_map;

    std::unordered_map<unsigned long long, unsigned long long>* seg_hash_map =
        (std::unordered_map<unsigned long long, unsigned long long>*)
            seg_hm.hash_map;

    node_hash_map->clear();
    seg_hash_map->clear();

    Zeta_SegVector_Check(&sv, &node_hm, &seg_hm);

    CheckFullContains(node_allocator_.records, *node_hash_map);

    CheckFullContains(seg_allocator_.records, *seg_hash_map);
}

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

    DD_Init();
    SV_Init();

    Zeta_DebugHashMap_Create(&node_hm);
    Zeta_DebugHashMap_Create(&seg_hm);

    Check();

    for (int i = 0; i < 1000; ++i) {
        size_t idx = size_generator(en) % (DD_GetSize() + 1);
        val_t val = val_generator(en);

        DD_Insert(idx, val);
        SV_Insert(idx, val);

        Check();
    }

    for (int _ = 0; _ < 100; ++_) {
        for (int i = 0; i < 2000; ++i) {
            size_t idx = size_generator(en) % (DD_GetSize() + 1);
            val_t val = val_generator(en);

            DD_Insert(idx, val);
            SV_Insert(idx, val);

            Check();
        }

        for (int i = 0; i < 2000; ++i) {
            size_t idx = size_generator(en) % DD_GetSize();

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
