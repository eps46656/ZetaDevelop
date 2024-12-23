#include <deque>
#include <iostream>
#include <map>
#include <random>
#include <unordered_set>

#include "../Zeta/dynamic_vector.h"
#include "../Zeta/utils.h"

struct Data {
    int x;
    int y;
};

bool_t operator==(Data const& a, Data const& b) {
    return a.x == b.x && a.y == b.y;
}

Data* Data_Init(void* context, void* data_) {
    ZETA_Unused(context);

    Data* data = (Data*)data_;
    return data;
}

void Data_Deinit(void* context, void* data_) {
    ZETA_Unused(context);
    Data* data = (Data*)data_;
    delete data;
}

using val_t = Data;

void PrintVal(val_t const& val) {
    std::cout << "( " << val.x << ", " << val.y << " )";
    // std::cout << val;
}

struct StdAllocator {
    std::unordered_map<void*, size_t> ptr_size;
};

size_t StdQuery(void* sa_, size_t size) {
    ZETA_Unused(sa_);
    return 0 < size ? size : 0;
}

void* StdAllocate(void* sa_, size_t size) {
    StdAllocator* sa = (StdAllocator*)sa_;

    if (size == 0) { return NULL; }

    void* ptr = malloc(size);
    sa->ptr_size.insert({ ptr, size });

    return ptr;
}

void StdDeallocate(void* sa_, void* ptr) {
    if (ptr == NULL) { return; }

    StdAllocator* sa = (StdAllocator*)sa_;

    bool_t b = sa->ptr_size.erase(ptr) != 0;
    ZETA_DebugAssert(b);

    free(ptr);
}

std::deque<val_t> de;
Zeta_DynamicVector dv;

std::mt19937_64 rand_en;

std::uniform_int_distribution<int> val_generator{ -64 * 1024 * 1024,
                                                  64 * 1024 * 1024 };

std::uniform_int_distribution<size_t> size_generator{ 0, ZETA_SIZE_MAX };

StdAllocator mlv_allocator;

Zeta_Allocator mlv_allocator_i = {
    .context = &mlv_allocator,
    .Query = StdQuery,
    .Allocate = StdAllocate,
    .Deallocate = StdDeallocate,
};

StdAllocator cluster_allocator;

Zeta_Allocator cluster_allocator_i = {
    .context = &cluster_allocator,
    .Query = StdQuery,
    .Allocate = StdAllocate,
    .Deallocate = StdDeallocate,
};

val_t GetVal() { return { val_generator(rand_en), val_generator(rand_en) }; }

size_t DvGetSize() { return Zeta_DynamicVector_GetSize(&dv); }

val_t* DvAccess(size_t idx) {
    return (val_t*)Zeta_DynamicVector_Access(&dv, idx);
}

val_t* DvInsert(size_t idx, val_t const& val) {
    void* val_p = Zeta_DynamicVector_Insert(&dv, idx);
    Zeta_MemCopy((byte_t*)val_p, (byte_t*)(void*)&val, sizeof(val_t));
    return (val_t*)val_p;
}

void DvErase(size_t idx) {
    // val_t* val = (val_t*)Zeta_DynamicVector_Access(&dv, idx);
    Zeta_DynamicVector_Erase(&dv, idx);
}

void DvEraseAll() {
    size_t size = DvGetSize();
    for (size_t i = size; 0 < i--;) { DvErase(i); }
}

void DvPrint() {
    size_t size = DvGetSize();

    std::cout << "{ ";

    for (size_t i = 0; i < size; ++i) {
        PrintVal(*DvAccess(i));
        std::cout << ", ";
    }

    std::cout << " }\n";
}

void SyncInsert(size_t idx, val_t const& val) {
    de.insert(de.begin() + idx, val);
    DvInsert(idx, val);
}

void SyncErase(size_t idx) {
    de.erase(de.begin() + idx);
    DvErase(idx);
}

void SyncEraseAll() {
    de.clear();
    DvEraseAll();
}

void Check() {
    ZETA_DebugAssert(DvGetSize() == de.size());

    size_t size = DvGetSize();

    for (size_t i = 0; i < size; ++i) {
        val_t* val_p = DvAccess(i);
        ZETA_DebugAssert(*val_p == de[i]);
    }

    Zeta_DebugHashTable* tm = (Zeta_DebugHashTable*)Zeta_DebugHashTable_Init();

    Zeta_MultiLevelVector_GetAllPages(&dv.mlv, tm);

    std::map<size_t, size_t>* m = (std::map<size_t, size_t>*)(tm);

    for (auto iter{ m->begin() }, end{ m->end() }; iter != end; ++iter) {
        ZETA_DebugAssert(
            mlv_allocator.ptr_size.find((void*)(uintptr_t)iter->first) !=
            mlv_allocator.ptr_size.end());
    }

    for (auto iter{ mlv_allocator.ptr_size.begin() },
         end{ mlv_allocator.ptr_size.end() };
         iter != end; ++iter) {
        ZETA_DebugAssert(m->find((uintptr_t)iter->first) != m->end());
    }
}

int main() {
    size_t seed = time(NULL);
    std::cout << "seed: " << seed << "\n";

    rand_en.seed(seed);

    dv.width = sizeof(val_t);
    dv.cluster_capacity = 8;
    dv.mlv.level = -1;
    dv.mlv.allocator = &mlv_allocator_i;
    dv.allocator = &cluster_allocator_i;

    Zeta_DynamicVector_Init(&dv);

    ZETA_PrintVar(dv.mlv.level);

    // for()

    for (int i = 0; i < 50; ++i) {
        Check();

        for (int j = 0, j_end = size_generator(rand_en) % 50 + 20; j <= j_end;
             ++j) {
            SyncInsert(size_generator(rand_en) % (de.size() + 1), GetVal());
        }

        Check();

        for (int j = 0, j_end = size_generator(rand_en) % 30 + 10; j <= j_end;
             ++j) {
            SyncErase(size_generator(rand_en) % de.size());
        }

        Check();
    }

    ZETA_PrintVar(mlv_allocator.ptr_size.size());
    ZETA_PrintVar(cluster_allocator.ptr_size.size());

    std::cout << "ok\n";

    return 0;
}
