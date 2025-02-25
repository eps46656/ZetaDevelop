#include <iostream>
#include <map>
#include <random>

#include "../Zeta/Slaballocator.h"
#include "../Zeta/debug_hash_table.h"
#include "std_allocator.h"

typedef long long unsigned llu;

StdAllocator std_allocator_;
Zeta_Allocator std_allocator;

Zeta_SlabAllocator allocator_;
Zeta_Allocator allocator;

std::vector<void*> ptrs;

void Check() {
    Zeta_DebugHashTable used_records;
    Zeta_DebugHashTable released_records;

    Zeta_DebugHashTable_Init(&used_records);
    Zeta_DebugHashTable_Init(&released_records);

    Zeta_SlabAllocator_Check(&allocator_, &used_records, &released_records);

    for (auto iter{ ptrs.begin() }, end{ ptrs.end() }; iter != end; ++iter) {
        bool_t b =
            Zeta_DebugHashTable_Erase(&released_records, ZETA_PtrToAddr(*iter));

        ZETA_DebugAssert(b);
    }

    ZETA_DebugAssert(Zeta_DebugHashTable_GetSize(&released_records) == 0);

    CheckFullContains(std_allocator_.records,
                      *(std::map<size_t, size_t>*)used_records.tree_map);
}

void main1() {
    unsigned seed = time(NULL);

    ZETA_PrintVar(seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> idx_generator{ 0, ZETA_RangeMaxOf(
                                                                size_t) };

    size_t width = sizeof(uintptr_t) * 3 + sizeof(size_t);

    allocator_.align = 8;
    allocator_.width = width;
    allocator_.num = 32;
    allocator_.allocator = &std_allocator;

    Zeta_SlabAllocator_Init(&allocator_);

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
