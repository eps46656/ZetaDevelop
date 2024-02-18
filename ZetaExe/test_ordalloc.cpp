#include <iostream>
#include <map>
#include <random>

#include "../Zeta/DebugTreeMap.h"
#include "../Zeta/OrdAllocator.h"

typedef long long unsigned llu;

byte_t mem[16 * 1024 * 1024];

Zeta_OrdAllocator allocator;

std::map<size_t, size_t> req_ptr_size_tm;

void CheckNoCross(const std::map<size_t, size_t>& m) {
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

void PrintTM(const std::map<size_t, size_t>& m) {
    std::cout << "{\n";

    for (auto iter{ m.begin() }, end{ m.end() }; iter != end; ++iter) {
        std::cout << "\t( " << iter->first << ", " << iter->second << " )\n";
    }

    std::cout << "}\n";
}

void* MyAlloc(size_t size) {
    if (size == 0) { return NULL; }

    ZETA_PrintPos;

    void* ptr = Zeta_OrdAllocator_Allocate(&allocator, size);

    ZETA_PrintPos;

    size_t m_beg = ZETA_PTR_TO_UINT(ptr);
    size_t m_end = m_beg + size;

    ZETA_DebugAssert(m_beg % allocator.align == 0);

    ZETA_DebugAssert(ZETA_PTR_TO_UINT(allocator.ptr) <= m_beg);
    ZETA_DebugAssert(m_end <= ZETA_PTR_TO_UINT(allocator.ptr) + allocator.size);

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
    ZETA_PrintPos;
    Zeta_OrdAllocator_Deallocate(&allocator, ptr);
    ZETA_PrintPos;
    size_t b = req_ptr_size_tm.erase((uintptr_t)ptr);
    ZETA_PrintPos;

    ZETA_DebugAssert(b);
}

void CheckAllocator(bool_t print_state) {
    void* ptr_size_tm_ = Zeta_DebugTreeMap_Create();

    std::map<size_t, size_t>& ptr_size_tm =
        *static_cast<std::map<size_t, size_t>*>(ptr_size_tm_);

    Zeta_OrdAllocator_Check(&allocator, print_state, ptr_size_tm_);

    // PrintTM(ptr_size_tm);

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

int main() {
    unsigned int seed = time(NULL);

    ZETA_PrintVar("%d", seed);

    std::mt19937_64 en{ seed };
    std::uniform_int_distribution<size_t> size_generator{ 16, 2 * 1024 };
    std::uniform_int_distribution<size_t> idx_generator{ 0,
                                                         ZETA_maxof(size_t) };

    allocator.ptr = mem;
    allocator.align = 16;
    allocator.size = sizeof(mem);
    allocator.num_of_slab_level = -1;

    Zeta_OrdAllocator_Init(&allocator);

    ZETA_DebugAssert(mem <= allocator.ptr);
    ZETA_DebugAssert(allocator.size <= sizeof(mem));

    ZETA_PrintPos;

    CheckAllocator(true);

    ZETA_PrintPos;

    std::vector<void*> ptrs;

    for (int test_i = 0; test_i < 50; ++test_i) {
        size_t size = size_generator(en);

        ptrs.push_back(MyAlloc(size));
    }

    ZETA_PrintPos;

    CheckAllocator(false);

    ZETA_PrintPos;

    for (int test_i = 0; test_i < 40; ++test_i) {
        size_t idx = idx_generator(en) % ptrs.size();

        ZETA_PrintPos;

        MyFree(ptrs[idx]);

        ZETA_PrintPos;

        ptrs[idx] = ptrs.back();
        ptrs.pop_back();
    }

    ZETA_PrintPos;

    CheckAllocator(false);

    ZETA_PrintPos;

    // Zeta_OrdAllocator_Check(&allocator, 1, ptr_size_tm);

    // DebugTreeMap_Print(ptr_size_tm);

    printf("ok\n");

    return 0;
}
