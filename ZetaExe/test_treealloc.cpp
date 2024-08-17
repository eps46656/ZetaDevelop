#include <iostream>
#include <map>
#include <random>

#include "../Zeta/DebugHashTable.h"
#include "../Zeta/Debugger.h"
#include "../Zeta/TreeAllocator.h"
#include "MemAllocatorCheck.h"

typedef long long unsigned llu;

byte_t mem[128 * 1024 * 1024];

Zeta_TreeAllocator allocator;

std::map<size_t, size_t> req_ptr_size_hm;

std::mt19937_64 en;
std::uniform_int_distribution<size_t> size_generator{ 16, 2 * 1024 };
std::uniform_int_distribution<size_t> idx_generator{ 0,
                                                     ZETA_RangeMaxOf(size_t) };

void CheckNoCross(std::unordered_map<size_t, size_t> const& m) {
    if (m.empty()) { return; }

    auto iter = m.begin();
    auto nxt_iter = std::next(iter);
    auto end = m.end();

    while (nxt_iter != end) {
        ZETA_DebugAssert(iter->first + iter->second <= nxt_iter->first);

        iter = nxt_iter;
        ++nxt_iter;
    }
}

void PrintTM(std::map<size_t, size_t> const& m) {
    std::cout << "{\n";

    for (auto iter{ m.begin() }, end{ m.end() }; iter != end; ++iter) {
        std::cout << "\t( " << iter->first << ", " << iter->second << " )\n";
    }

    std::cout << "}\n";
}

void* MyAlloc(size_t size) {
    if (size == 0) { return NULL; }

    void* ptr = Zeta_TreeAllocator_Allocate(&allocator, size);

    ZETA_DebugAssert(ptr != NULL);

    size_t m_beg = ZETA_PtrToAddr(ptr);
    size_t m_end = m_beg + size;

    ZETA_DebugAssert(m_beg % allocator.align == 0);

    ZETA_DebugAssert(ZETA_PtrToAddr(allocator.data_beg) <= m_beg);
    ZETA_DebugAssert(m_end <= ZETA_PtrToAddr(allocator.data_end));

    auto iter = req_ptr_size_hm.insert({ m_beg, size }).first;

    if (iter != req_ptr_size_hm.begin()) {
        auto pre_iter{ std::prev(iter) };
        ZETA_DebugAssert(pre_iter->first + pre_iter->second <= m_beg);
    }

    auto nxt_iter{ std::next(iter) };

    if (nxt_iter != req_ptr_size_hm.end()) {
        ZETA_DebugAssert(m_end <= nxt_iter->first);
    }

    for (size_t i{ 0 }; i < size; ++i) {
        *((unsigned char*)ptr + i) = size_generator(en) % 256;
    }

    return ptr;
}

void MyFree(void* ptr) {
    Zeta_TreeAllocator_Deallocate(&allocator, ptr);
    size_t b = req_ptr_size_hm.erase(ZETA_PtrToAddr(ptr));
    ZETA_DebugAssert(b);
}

void CheckAllocator(bool_t print_state) {
    Zeta_DebugHashTable records;
    Zeta_DebugHashTable_Init(&records);

    std::unordered_map<unsigned long long, unsigned long long>&
        records_hash_map = *static_cast<
            std::unordered_map<unsigned long long, unsigned long long>*>(
            records.hash_map);

    Zeta_TreeAllocator_Check(&allocator, print_state, &records);

    CheckNoOverlap(records_hash_map);
}

void main1() {
    unsigned seed = time(NULL);

    ZETA_PrintVar(seed);

    en.seed(seed);

    allocator.mem = mem;
    allocator.align = 16;
    allocator.size = sizeof(mem);

    Zeta_TreeAllocator_Init(&allocator);

    CheckAllocator(TRUE);

    std::vector<void*> ptrs;

    ZETA_PrintCurPos;

    for (int test_i = 0; test_i < 2000; ++test_i) {
        Zeta_Debugger_ClearPipe();

        ZETA_PrintCurPos;

        size_t size = size_generator(en);

        ZETA_PrintCurPos;

        ptrs.push_back(MyAlloc(size));

        ZETA_PrintCurPos;

        CheckAllocator(FALSE);
    }

    for (int _ = 0; _ < 100; ++_) {
        ZETA_PrintCurPos;

        for (int test_i = 0; test_i < 1000; ++test_i) {
            Zeta_Debugger_ClearPipe();

            ZETA_PrintCurPos;

            size_t size = size_generator(en);

            ptrs.push_back(MyAlloc(size));

            CheckAllocator(FALSE);
        }

        ZETA_PrintCurPos;

        for (int test_i = 0; test_i < 1000; ++test_i) {
            Zeta_Debugger_ClearPipe();

            ZETA_PrintCurPos;

            size_t idx = idx_generator(en) % ptrs.size();

            MyFree(ptrs[idx]);

            ptrs[idx] = ptrs.back();
            ptrs.pop_back();

            CheckAllocator(FALSE);
        }

        ZETA_PrintCurPos;
    }

    // Zeta_TreeAllocator_Check(&allocator, 1, ptr_size_hm);

    // _Print(ptr_size_hm);
}

int main() {
    main1();

    ZETA_PrintVar(ZETA_IsDebug);

    std::cout << "ok\n";

    return 0;
}
