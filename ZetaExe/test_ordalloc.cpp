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
        ZETA_DEBUG_ASSERT(iter->first + iter->second <= nxt_iter->first);

        iter = nxt_iter;
        ++nxt_iter;
    }
}

void PrintTM(const std::map<size_t, size_t>& m) {
    std::cout << "{\n";

    for (auto iter{m.begin()}, end{m.end()}; iter != end; ++iter) {
        std::cout << "\t( " << iter->first << ", " << iter->second << " )\n";
    }

    std::cout << "}\n";
}

void* MyAlloc(size_t size) {
    if (size == 0) { return NULL; }

    void* ptr = Zeta_OrdAllocator_Allocate(&allocator, size);

    size_t m_beg = (uintptr_t)ptr;
    size_t m_end = m_beg + size;

    auto iter = req_ptr_size_tm.insert({m_beg, size}).first;

    if (iter != req_ptr_size_tm.begin()) {
        auto pre_iter{std::prev(iter)};
        ZETA_DEBUG_ASSERT(pre_iter->first + pre_iter->second <= m_beg);
    }

    auto nxt_iter{std::next(iter)};

    if (nxt_iter != req_ptr_size_tm.end()) {
        ZETA_DEBUG_ASSERT(m_end <= nxt_iter->first);
    }

    return ptr;
}

void MyFree(void* ptr) {
    Zeta_OrdAllocator_Deallocate(&allocator, ptr);
    size_t b = req_ptr_size_tm.erase((uintptr_t)ptr);

    ZETA_DEBUG_ASSERT(b);
}

void CheckAllocator(bool_t print_state) {
    void* ptr_size_tm_ = DebugTreeMap_Create();

    std::map<size_t, size_t>& ptr_size_tm =
        *static_cast<std::map<size_t, size_t>*>(ptr_size_tm_);

    Zeta_OrdAllocator_Check(&allocator, print_state, ptr_size_tm_);

    PrintTM(ptr_size_tm);

    CheckNoCross(ptr_size_tm);

    for (auto iter{req_ptr_size_tm.begin()}, end{req_ptr_size_tm.end()};
         iter != end; ++iter) {
        auto jter = ptr_size_tm.find(iter->first);

        ZETA_DEBUG_ASSERT(jter != ptr_size_tm.end());
        ZETA_DEBUG_ASSERT(iter->second <= jter->second);
    }

    for (auto iter{ptr_size_tm.begin()}, end{ptr_size_tm.end()}; iter != end;
         ++iter) {
        auto jter = req_ptr_size_tm.find(iter->first);

        ZETA_DEBUG_ASSERT(jter != ptr_size_tm.end());
        ZETA_DEBUG_ASSERT(jter->second <= iter->second);
    }
}

int main() {
    unsigned int seed = time(NULL);

    ZETA_PRINT_VAR("%d", seed);

    std::mt19937_64 en{seed};
    std::uniform_int_distribution<size_t> dist{0, ZETA_MAXOF(size_t)};

    allocator.ptr = mem;
    allocator.align = 16;
    allocator.size = sizeof(mem);
    allocator.num_of_slab_level = -1;

    Zeta_OrdAllocator_Init(&allocator);

    CheckAllocator(true);

    std::vector<void*> ptrs;

    for (int i = 24; i <= 600; i += 32) { ptrs.push_back(MyAlloc(i)); }
    for (int i = 24; i <= 600; i += 32) { ptrs.push_back(MyAlloc(i)); }
    for (int i = 24; i <= 600; i += 32) { ptrs.push_back(MyAlloc(i)); }

    CheckAllocator(false);

    // Zeta_OrdAllocator_Check(&allocator, 1, ptr_size_tm);

    // DebugTreeMap_Print(ptr_size_tm);

    printf("ok\n");

    return 0;
}
