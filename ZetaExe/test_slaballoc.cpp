#include <iostream>
#include <map>
#include <random>

#include "../Zeta/DebugTreeMap.h"
#include "../Zeta/SlabAllocator.h"
#include "StdAllocator.h"

typedef long long unsigned llu;

StdAllocator std_allocator_;
Zeta_Allocator std_allocator;

Zeta_SlabAllocator allocator_;
Zeta_Allocator allocator;

std::map<size_t, size_t> req_ptr_size_tm;

void main1() {
    unsigned int seed = time(NULL);

    ZETA_PrintVar("%d", seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> idx_generator{ 0,
                                                         ZETA_maxof(size_t) };

    StdAllocator_ToAllocator(&std_allocator_, &std_allocator);

    size_t width = sizeof(uintptr_t) * 3 + sizeof(size_t);

    allocator_.align = 16;
    allocator_.width = width;
    allocator_.num = 32;
    allocator_.allocator = &std_allocator;

    Zeta_SlabAllocator_Init(&allocator_);

    Zeta_SlabAllocator_DeployAllocator(&allocator_, &allocator);

    std::vector<void*> ptrs;

    ZETA_PrintPos;

    for (int _ = 0; _ < 200; ++_) {
        for (int test_i = 0; test_i < 1000; ++test_i) {
            ptrs.push_back(allocator.Allocate(allocator.context, width));
        }

        for (int _ = 0; _ < 5; ++_) {
            for (int test_i = 0; test_i < 1000; ++test_i) {
                void* ptr = allocator.Allocate(allocator.context, width);

                ZETA_DebugAssert(ptr != NULL);

                ptrs.push_back(ptr);
            }

            for (int test_i = 0; test_i < 1000; ++test_i) {
                size_t idx = idx_generator(en) % ptrs.size();

                allocator.Deallocate(allocator.context, ptrs[idx]);

                ptrs[idx] = ptrs.back();
                ptrs.pop_back();
            }
        }

        for (; ptrs.size() != 0;) {
            size_t idx = idx_generator(en) % ptrs.size();

            allocator.Deallocate(allocator.context, ptrs[idx]);

            ptrs[idx] = ptrs.back();
            ptrs.pop_back();
        }

        ZETA_DebugAssert(std_allocator_.records.empty());
    }
}

int main() {
    main1();

    printf("ok\n");

    return 0;
}
