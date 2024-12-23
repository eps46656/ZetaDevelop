#include <debug_hash_table.h>
#include <debugger.h>
#include <mem_check_utils.h>
#include <tree_allocator.h>

#include <iostream>
#include <map>
#include <random>

typedef long long unsigned llu;

byte_t mem[128 * 1024 * 1024];

Zeta_TreeAllocator allocator;

Zeta_MemRecorder* mem_recorder;

std::map<size_t, size_t> req_ptr_size_hm;

std::mt19937_64 en;
std::uniform_int_distribution<size_t> size_generator{ 16, 2 * 1024 };
std::uniform_int_distribution<size_t> idx_generator{ 0, ZETA_SIZE_MAX };

void* MyAlloc(size_t size) {
    if (size == 0) { return NULL; }

    void* ptr = Zeta_TreeAllocator_Allocate(&allocator, size);

    ZETA_DebugAssert(ptr != NULL);

    Zeta_MemRecorder_Record(mem_recorder, ptr, size);

    return ptr;
}

void MyFree(void* ptr) {
    Zeta_TreeAllocator_Deallocate(&allocator, ptr);
    ZETA_DebugAssert(Zeta_MemRecorder_Unrecord(mem_recorder, ptr));
}

void CheckAllocator() {
    Zeta_MemRecorder* records{ Zeta_MemRecorder_Create() };

    Zeta_TreeAllocator_Sanitize(&allocator, records);

    Zeta_MemCheck_MatchRecords(records, mem_recorder);

    Zeta_MemRecorder_Destroy(records);
}

void main1() {
    unsigned seed = time(NULL);

    ZETA_PrintVar(seed);

    en.seed(seed);

    allocator.mem = mem;
    allocator.align = 16;
    allocator.size = sizeof(mem);

    Zeta_TreeAllocator_Init(&allocator);

    mem_recorder = Zeta_MemRecorder_Create();

    CheckAllocator();

    std::vector<void*> ptrs;

    ZETA_PrintCurPos;

    for (int test_i = 0; test_i < 2000; ++test_i) {
        Zeta_Debugger_ClearPipe();

        size_t size = size_generator(en);

        ptrs.push_back(MyAlloc(size));

        CheckAllocator();
    }

    for (int _ = 0; _ < 100; ++_) {
        ZETA_PrintVar(_);

        for (int test_i = 0; test_i < 1000; ++test_i) {
            Zeta_Debugger_ClearPipe();

            size_t size = size_generator(en);

            ptrs.push_back(MyAlloc(size));

            CheckAllocator();
        }

        ZETA_PrintCurPos;

        for (int test_i = 0; test_i < 1000; ++test_i) {
            Zeta_Debugger_ClearPipe();

            size_t idx = idx_generator(en) % ptrs.size();

            MyFree(ptrs[idx]);

            ptrs[idx] = ptrs.back();
            ptrs.pop_back();

            CheckAllocator();
        }

        ZETA_PrintCurPos;
    }

    // Zeta_TreeAllocator_Check(&allocator, 1, ptr_size_hm);

    // _Print(ptr_size_hm);
}

int main() {
    main1();

    std::cout << "ok\n";

    return 0;
}
