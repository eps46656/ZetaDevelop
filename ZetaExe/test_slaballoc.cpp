#include <iostream>
#include <map>
#include <random>

#include "../Zeta/DebugTreeMap.h"
#include "../Zeta/SlabAllocator.h"
#include "MemAllocatorCheck.h"
#include "StdAllocator.h"

typedef long long unsigned llu;

StdAllocator std_allocator_;
Zeta_Allocator std_allocator;

Zeta_SlabAllocator allocator_;
Zeta_Allocator allocator;

std::map<size_t, size_t> req_ptr_size_tm;

std::vector<void*> ptrs;

void Check() {
    Zeta_DebugTreeMap used_ptr_size_tm;
    Zeta_DebugTreeMap released_ptr_size_tm;

    Zeta_DebugTreeMap_Create(&used_ptr_size_tm);
    Zeta_DebugTreeMap_Create(&released_ptr_size_tm);

    Zeta_SlabAllocator_Check(&allocator_, &used_ptr_size_tm,
                             &released_ptr_size_tm);

    for (auto iter{ ptrs.begin() }, end{ ptrs.end() }; iter != end; ++iter) {
        bool_t b = Zeta_DebugTreeMap_Erase(&released_ptr_size_tm,
                                           ZETA_GetAddrFromPtr(*iter));

        ZETA_DebugAssert(b);
    }

    ZETA_DebugAssert(Zeta_DebugTreeMap_GetSize(&released_ptr_size_tm) == 0);

    CheckFullContains(std_allocator_.records,
                      *(std::map<size_t, size_t>*)used_ptr_size_tm.tree_map);
}

void main1() {
    unsigned int seed = time(NULL);

    ZETA_PrintVar(seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> idx_generator{ 0, ZETA_GetRangeMax(
                                                                size_t) };

    StdAllocator_DeployAllocator(&std_allocator_, &std_allocator);

    size_t width = sizeof(uintptr_t) * 3 + sizeof(size_t);

    allocator_.align = 16;
    allocator_.width = width;
    allocator_.num = 32;
    allocator_.allocator = &std_allocator;

    Zeta_SlabAllocator_Init(&allocator_);

    Zeta_SlabAllocator_DeployAllocator(&allocator_, &allocator);

    ZETA_PrintPos;

    ZETA_PrintVar(allocator_.width);

    for (int _ = 0; _ < 5; ++_) {
        for (int test_i = 0; test_i < 1000; ++test_i) {
            ptrs.push_back(allocator.Allocate(allocator.context, width));
            Check();
        }

        Check();

        for (int _ = 0; _ < 10; ++_) {
            for (int test_i = 0; test_i < 1000; ++test_i) {
                void* ptr = allocator.Allocate(allocator.context, width);

                ZETA_DebugAssert(ptr != NULL);

                ptrs.push_back(ptr);

                Check();
            }

            for (int test_i = 0; test_i < 1000; ++test_i) {
                size_t idx = idx_generator(en) % ptrs.size();

                allocator.Deallocate(allocator.context, ptrs[idx]);

                ptrs[idx] = ptrs.back();
                ptrs.pop_back();

                Check();
            }
        }

        Check();

        for (; ptrs.size() != 0;) {
            size_t idx = idx_generator(en) % ptrs.size();

            allocator.Deallocate(allocator.context, ptrs[idx]);

            ptrs[idx] = ptrs.back();
            ptrs.pop_back();

            Check();
        }

        Check();

        ZETA_DebugAssert(std_allocator_.records.empty());
    }
}

int main() {
    main1();

    printf("ok\n");

    return 0;
}
