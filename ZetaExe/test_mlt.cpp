#include <multi_level_table.h>

#include <map>

#include "random.h"
#include "std_allocator.h"
#include "timer.h"

struct ZetaMap {
    StdAllocator node_allocator_;
    Zeta_Allocator node_allocator;

    Zeta_MultiLevelTable mlt;

    ZetaMap() {
        ZETA_PrintCurPos;

        int level{ 8 };

        this->mlt.level = level;

        ZETA_PrintCurPos;

        for (int level_i{ 0 }; level_i < level; ++level_i) {
            this->mlt.branch_nums[level_i] =
                ZETA_MultiLevelTable_max_branch_num;
        }

        ZETA_PrintCurPos;

        StdAllocator_DeployAllocator(&this->node_allocator_,
                                     &this->node_allocator);

        this->mlt.node_allocator = &this->node_allocator;

        Zeta_MultiLevelTable_Init(&this->mlt);

        ZETA_PrintCurPos;

        this->Sanitize();
    }

    void SetIdxes_(size_t idx, size_t* dst_idxes) {
        int level = this->mlt.level;

        for (int level_i{ level - 1 }; 0 <= level_i; --level_i) {
            dst_idxes[level_i] = idx % this->mlt.branch_nums[level_i];
            idx /= this->mlt.branch_nums[level_i];
        }
    }

    size_t GetIdx_(size_t* idxes) {
        int level = this->mlt.level;

        size_t idx = 0;

        for (int level_i{ 0 }; level_i < level; ++level_i) {
            idx = idx * this->mlt.branch_nums[level_i] + idxes[level_i];
        }

        return idx;
    }

    void Sanitize() {
        Zeta_MemRecorder* node = Zeta_MemRecorder_Create();

        Zeta_MultiLevelTable_Sanitize(&this->mlt, node);

        Zeta_MemCheck_MatchRecords(this->node_allocator_.mem_recorder, node);

        Zeta_MemRecorder_Destroy(node);
    }

    size_t GetSize() { return Zeta_MultiLevelTable_GetSize(&this->mlt); }

    size_t GetCapacity() {
        return Zeta_MultiLevelTable_GetCapacity(&this->mlt);
    }

    void* Access(size_t idx) {
        void** n = Zeta_MultiLevelTable_Access(&this->mlt, idx);

        this->Sanitize();

        return n == NULL ? NULL : *n;
    }

    void Insert(size_t idx, void* val) {
        size_t idxes[ZETA_MultiLevelTable_max_level];

        this->SetIdxes_(idx, idxes);

        void** n = Zeta_MultiLevelTable_Insert(&this->mlt, idxes);

        ZETA_DebugAssert(n != NULL);

        *n = val;

        this->Sanitize();
    }

    void Erase(size_t idx) {
        size_t idxes[ZETA_MultiLevelTable_max_level];

        this->SetIdxes_(idx, idxes);

        Zeta_MultiLevelTable_Erase(&this->mlt, idxes);

        this->Sanitize();
    }

    size_t FindPrev(size_t idx) {
        void** n = Zeta_MultiLevelTable_FindPrev(&this->mlt, &idx);
        ZETA_Unused(n);

        this->Sanitize();

        return idx;
    }

    size_t FindNext(size_t idx) {
        void** n = Zeta_MultiLevelTable_FindNext(&this->mlt, &idx);
        ZETA_Unused(n);

        this->Sanitize();

        return idx;
    }
};

struct StdMap {
    std::map<size_t, void*> m;

    size_t GetSize() { return this->m.size(); }

    void* Access(size_t idx) {
        auto iter{ this->m.find(idx) };
        return iter == m.end() ? NULL : iter->second;
    }

    void Insert(size_t idx, void* val) { this->m[idx] = val; }

    void Erase(size_t idx) { this->m.erase(idx); }

    size_t FindPrev(size_t idx) {
        auto iter{ this->m.upper_bound(idx) };
        return iter == this->m.begin() ? -1 : std::prev(iter)->first;
    }

    size_t FindNext(size_t idx) {
        auto iter{ this->m.lower_bound(idx) };
        return iter == this->m.end() ? -1 : iter->first;
    }
};

char base;

void SyncAccess(ZetaMap& zeta_map, StdMap& std_map) {
    size_t capacity{ zeta_map.GetCapacity() };

    size_t idx{ GetRandomInt<size_t, size_t>(0, capacity - 1) };

    ZETA_DebugAssert(zeta_map.Access(idx) == std_map.Access(idx));
}

void SyncInsert(ZetaMap& zeta_map, StdMap& std_map) {
    size_t capacity{ zeta_map.GetCapacity() };

    size_t idx{ GetRandomInt<size_t, size_t>(0, capacity - 1) };

    void* val{ &base + GetRandomInt<size_t, size_t>(0, 1024 * 1024) };

    zeta_map.Insert(idx, val);
    ZETA_DebugAssert(zeta_map.Access(idx) == val);

    std_map.Insert(idx, val);
    ZETA_DebugAssert(std_map.Access(idx) == val);
}

void SyncErase(ZetaMap& zeta_map, StdMap& std_map) {
    size_t capacity{ zeta_map.GetCapacity() };

    size_t idx{ GetRandomInt<size_t, size_t>(0, capacity - 1) };

    zeta_map.Erase(idx);
    ZETA_DebugAssert(zeta_map.Access(idx) == NULL);

    std_map.Erase(idx);
    ZETA_DebugAssert(std_map.Access(idx) == NULL);
}

void SyncFindPrev(ZetaMap& zeta_map, StdMap& std_map) {
    size_t capacity{ zeta_map.GetCapacity() };

    size_t idx{ GetRandomInt<size_t, size_t>(0, capacity - 1) };

    size_t zeta_map_prv_idx{ zeta_map.FindPrev(idx) };

    size_t std_map_prv_idx{ std_map.FindPrev(idx) };

    ZETA_DebugAssert(zeta_map_prv_idx == std_map_prv_idx);

    ZETA_DebugAssert(zeta_map.Access(zeta_map_prv_idx) ==
                     std_map.Access(std_map_prv_idx));
}

void SyncFindNext(ZetaMap& zeta_map, StdMap& std_map) {
    size_t capacity{ zeta_map.GetCapacity() };

    size_t idx{ GetRandomInt<size_t, size_t>(0, capacity - 1) };

    size_t zeta_map_prv_idx{ zeta_map.FindNext(idx) };

    size_t std_map_prv_idx{ std_map.FindNext(idx) };

    ZETA_DebugAssert(zeta_map_prv_idx == std_map_prv_idx);

    ZETA_DebugAssert(zeta_map.Access(zeta_map_prv_idx) ==
                     std_map.Access(std_map_prv_idx));
}

#define FOR_LOOP_(tmp_end, var, beg, end) \
    for (auto var = (beg), tmp_end = (end); var != tmp_end; ++var)

#define FOR_LOOP(var, beg, end) FOR_LOOP_(ZETA_TmpName, var, (beg), (end))

void main1() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1729615114;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    RandomEngine().seed(seed);

    ZetaMap zeta_map;
    StdMap std_map;

    FOR_LOOP(insert_i, 0, 1024) {
        ZETA_PrintVar(insert_i);
        SyncInsert(zeta_map, std_map);
    }

    FOR_LOOP(test_i, 0, 16) {
        ZETA_PrintVar(test_i);

        FOR_LOOP(ZETA_TmpName, 0, 1024) { SyncInsert(zeta_map, std_map); }

        FOR_LOOP(ZETA_TmpName, 0, 1024) { SyncErase(zeta_map, std_map); }

        FOR_LOOP(ZETA_TmpName, 0, 1024) { SyncFindPrev(zeta_map, std_map); }

        FOR_LOOP(ZETA_TmpName, 0, 1024) { SyncFindNext(zeta_map, std_map); }
    }
}

int main() {
    unsigned long long beg_time{ GetTime() };
    ZETA_PrintVar(beg_time);

    main1();

    ZETA_PrintVar(beg_time);

    unsigned long long end_time{ GetTime() };
    ZETA_PrintVar(end_time);

    unsigned long long duration{ end_time - beg_time };
    ZETA_PrintVar(duration);

    return 0;
}
