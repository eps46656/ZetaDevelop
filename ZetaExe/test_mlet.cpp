#include <random>
#include <unordered_map>
#include <vector>

#include "../Zeta/MultiLevelEntryTable.h"

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

StdAllocator mlet_allocator;

Zeta_Allocator mlet_allocator_i = {
    .context = &mlet_allocator,
    .Query = StdQuery,
    .Allocate = StdAllocate,
    .Deallocate = StdDeallocate,
};

Zeta_MultiLevelEntryTable mlet;

std::vector<void*> vet;

std::mt19937_64 rand_en;

std::uniform_int_distribution<size_t> size_generator{ 0, ZETA_maxof(size_t) };

std::uniform_int_distribution<int> val_generator{ -64 * 1024 * 1024,
                                                  64 * 1024 * 1024 };

void* KULL = (void*)~0;

size_t GetIdx(const size_t* idxes) {
    size_t idx = idxes[0];

    for (int level_i = 1; level_i < mlet.level; ++level_i) {
        idx = idx * mlet.branch_nums[level_i] + idxes[level_i];
    }

    return idx;
}

void GetIdxes(size_t* dst, size_t idx) {
    size_t origin_idx = idx;

    for (int level_i = mlet.level - 1; 0 <= level_i; --level_i) {
        dst[level_i] = idx % mlet.branch_nums[level_i];
        idx /= mlet.branch_nums[level_i];
    }

    ZETA_DebugAssert(origin_idx == GetIdx(dst));
}

void* MLETGetVal(size_t idx) {
    size_t idxes[ZETA_MultiLevelEntryTable_max_level];
    GetIdxes(idxes, idx);
    void** p = Zeta_MultiLevelEntryTable_Access(&mlet, idxes);
    return p == NULL ? NULL : *p;
}

void MLETSetVal(size_t idx, void* val) {
    size_t idxes[ZETA_MultiLevelEntryTable_max_level];
    GetIdxes(idxes, idx);

    if (val == NULL) {
        Zeta_MultiLevelEntryTable_Erase(&mlet, idxes);
    } else {
        *Zeta_MultiLevelEntryTable_Insert(&mlet, idxes) = val;
    }
}

size_t MLETFindPrevCri(size_t idx, bool_t included) {
    if (!included) {
        if (idx == 0) { return ZETA_maxof(size_t); }
        --idx;
    }

    for (++idx; 1 <= idx--;) {
        if (MLETGetVal(idx) != NULL) { return idx; }
    }

    return ZETA_maxof(size_t);
}

size_t MLETFindNextCri(size_t idx, bool_t included) {
    size_t capacity = Zeta_MultiLevelEntryTable_GetCapacity(&mlet);

    if (!included) {
        if (idx == capacity - 1) { return ZETA_maxof(size_t); }

        ++idx;
    }

    for (; idx < capacity; ++idx) {
        if (MLETGetVal(idx) != NULL) { return idx; }
    }

    return ZETA_maxof(size_t);
}

size_t MLETFindPrev(size_t idx, bool_t included) {
    size_t idxes[ZETA_MultiLevelEntryTable_max_level];
    GetIdxes(idxes, idx);

    void** p =
        Zeta_MultiLevelEntryTable_FindPrevNotNull(&mlet, idxes, included);

    return p == NULL ? ZETA_maxof(size_t) : GetIdx(idxes);
}

size_t MLETFindNext(size_t idx, bool_t included) {
    size_t idxes[ZETA_MultiLevelEntryTable_max_level];
    GetIdxes(idxes, idx);

    void** p =
        Zeta_MultiLevelEntryTable_FindNextNotNull(&mlet, idxes, included);

    return p == NULL ? ZETA_maxof(size_t) : GetIdx(idxes);
}

void MLETEraseAll() { Zeta_MultiLevelEntryTable_EraseAll(&mlet); }

void SyncSetVal(size_t idx, void* val) {
    vet[idx] = val;
    MLETSetVal(idx, val);
}

void SyncEraseAll() {
    size_t capacity = Zeta_MultiLevelEntryTable_GetCapacity(&mlet);

    vet.resize(0);
    vet.resize(capacity, NULL);

    MLETEraseAll();
}

void Check() {
    size_t capacity = vet.size();

    ZETA_DebugAssert(Zeta_MultiLevelEntryTable_IsClean(&mlet));

    for (size_t i = 0; i < capacity; ++i) {
        ZETA_DebugAssert(MLETGetVal(i) == vet[i]);
    }
}

#define TO_STR(x) #x

int main1() {
    int seed = time(NULL);
    printf("seed: %d\n", seed);

    rand_en.seed(seed);

    mlet.level = 6;
    mlet.branch_nums[0] = 4;
    mlet.branch_nums[1] = 3;
    mlet.branch_nums[2] = 4;
    mlet.branch_nums[3] = 6;
    mlet.branch_nums[4] = 5;
    mlet.branch_nums[5] = 2;
    mlet.allocator = &mlet_allocator_i;

    Zeta_MultiLevelEntryTable_Init(&mlet);

    Check();

    size_t capacity = Zeta_MultiLevelEntryTable_GetCapacity(&mlet);

    ZETA_PrintVar("%llu", capacity);
    vet.resize(capacity, NULL);

    for (int test_i = 0; test_i < 16; ++test_i) {
        for (size_t i = 0; i < 2; ++i) {
            size_t idx = size_generator(rand_en) % capacity;
            void* val = val_generator(rand_en) < 0 ? NULL : KULL;
            SyncSetVal(idx, val);
        }

        ZETA_PrintVar("%d", test_i);

        Check();

        ZETA_PrintVar("%d", test_i);

        for (size_t i = 0; i < capacity; ++i) {
            ZETA_DebugAssert(MLETFindPrev(i, TRUE) == MLETFindPrevCri(i, TRUE));

            ZETA_DebugAssert(MLETFindPrev(i, FALSE) ==
                             MLETFindPrevCri(i, FALSE));

            ZETA_DebugAssert(MLETFindNext(i, TRUE) == MLETFindNextCri(i, TRUE));

            ZETA_DebugAssert(MLETFindNext(i, FALSE) ==
                             MLETFindNextCri(i, FALSE));
        }

        SyncEraseAll();

        ZETA_DebugAssert(mlet_allocator.ptr_size.empty());
    }

    Check();

    printf("ok\n");

    return 0;
}

int main() { return main1(); }
