#include <map>
#include <random>
#include <unordered_map>
#include <vector>

#include "../Zeta/MultiLevelVector.h"
#include "StdAllocator.h"

StdAllocator mlv_node_allocator_;
StdAllocator mlv_table_allocator_;

Zeta_Allocator mlv_node_allocator;
Zeta_Allocator mlv_table_allocator;

Zeta_MultiLevelVector mlv;

std::map<size_t, void*> vet;

std::mt19937_64 rand_en;

std::uniform_int_distribution<size_t> size_generator{ 0, ZETA_GetRangeMax(
                                                             size_t) };

std::uniform_int_distribution<size_t> val_generator{ 1, 64 * 1024 * 1024 };

void* KULL = (void*)~0;

size_t GetIdx(size_t const* idxes) {
    size_t idx = idxes[0];

    for (int level_i = 1; level_i < mlv.level; ++level_i) {
        idx = idx * mlv.branch_nums[level_i] + idxes[level_i];
    }

    return idx;
}

void GetIdxes(size_t* dst, size_t idx) {
    size_t origin_idx = idx;

    for (int level_i = mlv.level - 1; 0 <= level_i; --level_i) {
        dst[level_i] = idx % mlv.branch_nums[level_i];
        idx /= mlv.branch_nums[level_i];
    }

    ZETA_DebugAssert(origin_idx == GetIdx(dst));
}

void MLV_Init() {
    Zeta_Allocator_Init(&mlv_node_allocator);
    Zeta_Allocator_Init(&mlv_table_allocator);

    StdAllocator_DeployAllocator(&mlv_node_allocator_, &mlv_node_allocator);
    StdAllocator_DeployAllocator(&mlv_table_allocator_, &mlv_table_allocator);

    mlv.level = 2;
    mlv.branch_nums[0] = 128;
    mlv.branch_nums[1] = 64;
    mlv.node_allocator = &mlv_node_allocator;
    mlv.table_allocator = &mlv_table_allocator;

    Zeta_MultiLevelVector_Init(&mlv);
}

void* MLV_GetVal(size_t idx) {
    size_t idxes[ZETA_MultiLevelVector_max_level];
    GetIdxes(idxes, idx);
    void** p = Zeta_MultiLevelVector_Access(&mlv, idxes);
    return p == NULL ? NULL : *p;
}

void MLV_SetVal(size_t idx, void* val) {
    size_t idxes[ZETA_MultiLevelVector_max_level];
    GetIdxes(idxes, idx);

    *Zeta_MultiLevelVector_Insert(&mlv, idxes) = val;
}

void MLV_Erase(size_t idx) {
    size_t idxes[ZETA_MultiLevelVector_max_level];
    GetIdxes(idxes, idx);

    Zeta_MultiLevelVector_Erase(&mlv, idxes);
}

size_t FindPrevCri(size_t idx, bool_t included) {
    if (!included) {
        if (idx == 0) { return ZETA_GetRangeMax(size_t); }
        --idx;
    }

    auto iter{ vet.upper_bound(idx) };

    return iter == vet.begin() ? ZETA_GetRangeMax(size_t)
                               : std::prev(iter)->first;
}

size_t FindNextCri(size_t idx, bool_t included) {
    size_t capacity = Zeta_MultiLevelVector_GetCapacity(&mlv);

    if (!included) {
        if (idx == capacity - 1) { return ZETA_GetRangeMax(size_t); }
        ++idx;
    }

    auto iter{ vet.lower_bound(idx) };

    return iter == vet.end() ? ZETA_GetRangeMax(size_t) : iter->first;
}

size_t MLVFindPrev(size_t idx, bool_t included) {
    size_t idxes[ZETA_MultiLevelVector_max_level];
    GetIdxes(idxes, idx);

    void** p = Zeta_MultiLevelVector_FindPrevNotNull(&mlv, idxes, included);

    if (p == NULL) { return ZETA_GetRangeMax(size_t); }

    ZETA_DebugAssert(p == Zeta_MultiLevelVector_Access(&mlv, idxes));

    return GetIdx(idxes);
}

size_t MLVFindNext(size_t idx, bool_t included) {
    size_t idxes[ZETA_MultiLevelVector_max_level];
    GetIdxes(idxes, idx);

    void** p = Zeta_MultiLevelVector_FindNextNotNull(&mlv, idxes, included);

    if (p == NULL) { return ZETA_GetRangeMax(size_t); }

    ZETA_DebugAssert(p == Zeta_MultiLevelVector_Access(&mlv, idxes));

    return GetIdx(idxes);
}

void MLVEraseAll() { Zeta_MultiLevelVector_EraseAll(&mlv); }

void SyncSetVal(size_t idx, void* val) {
    vet[idx] = val;
    MLV_SetVal(idx, val);
}

void SyncErase(size_t idx) {
    vet.erase(idx);
    MLV_Erase(idx);
}

void SyncEraseAll() {
    vet.clear();

    MLVEraseAll();
}

void Check() {
    ZETA_DebugAssert(vet.size() == Zeta_MultiLevelVector_GetSize(&mlv));

    size_t capacity = Zeta_MultiLevelVector_GetCapacity(&mlv);

    for (size_t i = 0; i < capacity; ++i) {
        auto iter{ vet.find(i) };
        void* x = iter == vet.end() ? NULL : iter->second;
        ZETA_DebugAssert(MLV_GetVal(i) == x);
    }

    Zeta_DebugTreeMap node_records;
    Zeta_DebugTreeMap table_records;

    Zeta_DebugTreeMap_Create(&node_records);
    Zeta_DebugTreeMap_Create(&table_records);

    Zeta_MultiLevelVector_GetAllPages(&mlv, &node_records, &table_records);

    StdAllocator_CheckRecords(&mlv_node_allocator_, &node_records);
    StdAllocator_CheckRecords(&mlv_table_allocator_, &table_records);

    Zeta_DebugTreeMap_Destroy(&node_records);
    Zeta_DebugTreeMap_Destroy(&table_records);
}

void main1() {
    // time_t seed = time(NULL);
    time_t seed = 1712994798;

    printf("seed: %d\n", seed);

    rand_en.seed(seed);

    MLV_Init();

    Check();

    size_t capacity = Zeta_MultiLevelVector_GetCapacity(&mlv);

    for (int test_i = 0; test_i < 16; ++test_i) {
        ZETA_PrintVar(test_i);

        for (size_t i = 0; i < 2000; ++i) {
            size_t idx = size_generator(rand_en) % capacity;
            void* val = (void*)val_generator(rand_en);
            SyncSetVal(idx, val);
            Check();
        }

        ZETA_PrintPos;

        for (int test_j = 0; test_j < 20; ++test_j) {
            ZETA_PrintVar(test_i);
            ZETA_PrintVar(test_j);

            for (size_t i = 0; i < 2000; ++i) {
                size_t idx = size_generator(rand_en) % capacity;
                void* val = (void*)val_generator(rand_en);
                SyncSetVal(idx, val);
                Check();
            }

            ZETA_PrintPos;

            for (size_t i = 0; i < capacity; ++i) {
                ZETA_DebugAssert(MLVFindPrev(i, TRUE) == FindPrevCri(i, TRUE));
                ZETA_DebugAssert(MLVFindPrev(i, FALSE) ==
                                 FindPrevCri(i, FALSE));

                ZETA_DebugAssert(MLVFindNext(i, TRUE) == FindNextCri(i, TRUE));
                ZETA_DebugAssert(MLVFindNext(i, FALSE) ==
                                 FindNextCri(i, FALSE));
            }

            ZETA_PrintPos;

            for (size_t i = 0; i < 2000; ++i) {
                size_t idx = size_generator(rand_en) % capacity;
                SyncErase(idx);
                Check();
            }

            ZETA_PrintPos;

            for (size_t i = 0; i < capacity; ++i) {
                ZETA_DebugAssert(MLVFindPrev(i, TRUE) == FindPrevCri(i, TRUE));
                ZETA_DebugAssert(MLVFindPrev(i, FALSE) ==
                                 FindPrevCri(i, FALSE));
                ZETA_DebugAssert(MLVFindNext(i, TRUE) == FindNextCri(i, TRUE));
                ZETA_DebugAssert(MLVFindNext(i, FALSE) ==
                                 FindNextCri(i, FALSE));
            }
        }

        SyncEraseAll();
    }

    Check();
}

int main() {
    main1();
    printf("ok\n");
    return 0;
}
