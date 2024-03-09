#include <iostream>
#include <map>
#include <random>

#include "../Zeta/DebugTreeMap.h"
#include "../Zeta/OrdAllocator.h"

typedef long long unsigned llu;

byte_t mem[128 * 1024 * 1024];

Zeta_OrdAllocator allocator;

std::map<size_t, size_t> req_ptr_size_tm;

void CheckNoCross(std::map<size_t, size_t> const& m) {
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

    void* ptr = Zeta_OrdAllocator_Allocate(&allocator, size);

    ZETA_DebugAssert(ptr != NULL);

    size_t m_beg = ZETA_PTR_TO_UINT(ptr);
    size_t m_end = m_beg + size;

    ZETA_DebugAssert(m_beg % allocator.align == 0);

    ZETA_DebugAssert(ZETA_PTR_TO_UINT(allocator.data_beg) <= m_beg);
    ZETA_DebugAssert(m_end <= ZETA_PTR_TO_UINT(allocator.data_end));

    auto iter = req_ptr_size_tm.insert({ m_beg, size }).first;

    if (iter != req_ptr_size_tm.begin()) {
        auto pre_iter{ std::prev(iter) };
        ZETA_DebugAssert(pre_iter->first + pre_iter->second <= m_beg);
    }

    auto nxt_iter{ std::next(iter) };

    if (nxt_iter != req_ptr_size_tm.end()) {
        ZETA_DebugAssert(m_end <= nxt_iter->first);
    }

    return ptr;
}

void MyFree(void* ptr) {
    Zeta_OrdAllocator_Deallocate(&allocator, ptr);

    size_t b = req_ptr_size_tm.erase((uintptr_t)ptr);

    ZETA_DebugAssert(b);
}

void CheckAllocator(bool_t print_state) {
    ZETA_Unused(print_state);

    Zeta_DebugTreeMap ptr_size_tm_;
    Zeta_DebugTreeMap_Create(&ptr_size_tm_);

    std::map<size_t, size_t>& ptr_size_tm =
        *static_cast<std::map<size_t, size_t>*>(ptr_size_tm_.tree_map);

    Zeta_OrdAllocator_Check(&allocator, &ptr_size_tm_);

    CheckNoCross(ptr_size_tm);

    for (auto iter{ req_ptr_size_tm.begin() }, end{ req_ptr_size_tm.end() };
         iter != end; ++iter) {
        auto jter = ptr_size_tm.find(iter->first);

        ZETA_DebugAssert(jter != ptr_size_tm.end());
        ZETA_DebugAssert(iter->second <= jter->second);
    }

    for (auto iter{ ptr_size_tm.begin() }, end{ ptr_size_tm.end() };
         iter != end; ++iter) {
        auto jter = req_ptr_size_tm.find(iter->first);

        ZETA_DebugAssert(jter != ptr_size_tm.end());
        ZETA_DebugAssert(jter->second <= iter->second);
    }
}

void main1() {
    unsigned int seed = time(NULL);

    ZETA_PrintVar("%d", seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> size_generator{ 16, 2 * 1024 };
    std::uniform_int_distribution<size_t> idx_generator{ 0,
                                                         ZETA_maxof(size_t) };

    allocator.mem = mem;
    allocator.align = 16;
    allocator.size = sizeof(mem);
    allocator.num_of_slab_level = -1;

    Zeta_OrdAllocator_Init(&allocator);

    CheckAllocator(true);

    std::vector<void*> ptrs;

    ZETA_PrintPos;

    for (int test_i = 0; test_i < 2000; ++test_i) {
        size_t size = size_generator(en);

        ptrs.push_back(MyAlloc(size));

        CheckAllocator(false);
    }

    for (int _ = 0; _ < 100; ++_) {
        for (int test_i = 0; test_i < 1000; ++test_i) {
            size_t size = size_generator(en);

            ptrs.push_back(MyAlloc(size));

            CheckAllocator(false);
        }

        ZETA_PrintPos;

        for (int test_i = 0; test_i < 1000; ++test_i) {
            size_t idx = idx_generator(en) % ptrs.size();

            MyFree(ptrs[idx]);

            ptrs[idx] = ptrs.back();
            ptrs.pop_back();

            CheckAllocator(false);
        }
    }

    // Zeta_OrdAllocator_Check(&allocator, ptr_size_tm);

    // DebugTreeMap_Print(ptr_size_tm);
}

int main() {
    main1();

    printf("ok\n");

    return 0;
}
