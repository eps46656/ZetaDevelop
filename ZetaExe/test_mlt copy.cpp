#include <chrono>
#include <deque>
#include <map>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>

#include "../Zeta/MultiLevelTable.h"
#include "../Zeta/OrdLinkedListNode.h"
#include "CppStdAllocator.h"
#include "ZetaPoolAllocator.h"

size_t time_ring{ (size_t)1 << 32 };
size_t tasks_num{ 1024 * 1024 };

std::mt19937_64 rand_en;

std::uniform_int_distribution<size_t> time_generator{ 0, time_ring - 1 };

std::uniform_int_distribution<size_t> time_stride_generator{ 20, 200000 };

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

size_t GetIdx(Zeta_MultiLevelTable* mlt, size_t const* idxes) {
    size_t idx = idxes[0];

    for (int level_i = 1; level_i < mlt->level; ++level_i) {
        idx = idx * mlt->branch_nums[level_i] + idxes[level_i];
    }

    return idx;
}

void GetIdxes(Zeta_MultiLevelTable* mlt, size_t* dst, size_t idx) {
    size_t origin_idx = idx;

    for (int level_i = mlt->level - 1; 0 <= level_i; --level_i) {
        dst[level_i] = idx % mlt->branch_nums[level_i];
        idx /= mlt->branch_nums[level_i];
    }

    ZETA_DebugAssert(origin_idx == GetIdx(mlt, dst));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct MLTTaskNode {
    size_t id;
    Zeta_OrdLinkedListNode n;
};

class MLTScheduler {
public:
    size_t time_capacity;
    size_t tasks_num;

    std::vector<MLTTaskNode> task_nodes;

    ZetaPoolAllocator mlt_node_allocator_;
    Zeta_Allocator mlt_node_allocator;

    Zeta_MultiLevelTable mlt;

    MLTScheduler(size_t time_capacity, size_t tasks_num);

    void PushTask(size_t time, size_t task_id);

    std::pair<size_t, size_t> PopTask(size_t time);
};

MLTScheduler::MLTScheduler(size_t time_capacity, size_t tasks_num)
    : time_capacity{ time_capacity }, tasks_num{ tasks_num } {
    this->task_nodes.resize(this->tasks_num);

    for (size_t task_id{ 0 }; task_id < this->tasks_num; ++task_id) {
        this->task_nodes[task_id].id = task_id;

        Zeta_OrdLinkedListNode_Init(&this->task_nodes[task_id].n);
    }

    size_t needed_capacity{ this->time_capacity };

    int level_i = 0;

    for (; 1 < needed_capacity;
         needed_capacity /= ZETA_MultiLevelTable_max_branch_num, ++level_i) {
        this->mlt.branch_nums[level_i] = ZETA_MultiLevelTable_max_branch_num;
    }

    this->mlt.level = level_i;

    ZETA_PrintVar(this->mlt.level);

    this->mlt_node_allocator_.size = 512 + 8;
    this->mlt_node_allocator_.max_buffered_ptrs_num = tasks_num * level_i;

    Zeta_Allocator_Init(&this->mlt_node_allocator);

    ZetaPoolAllocator_DeployAllocator(&this->mlt_node_allocator_,
                                      &this->mlt_node_allocator);

    this->mlt.node_allocator = &this->mlt_node_allocator;

    Zeta_MultiLevelTable_Init(&this->mlt);
}

void MLTScheduler::PushTask(size_t time, size_t task_id) {
    size_t idxes[ZETA_MultiLevelTable_max_level];
    GetIdxes(&this->mlt, idxes, time);

    size_t old_size{ Zeta_MultiLevelTable_GetSize(&this->mlt) };

    void** ele{ Zeta_MultiLevelTable_Insert(&this->mlt, idxes) };

    size_t new_size{ Zeta_MultiLevelTable_GetSize(&this->mlt) };

    if (old_size == new_size) {
        Zeta_OrdLinkedListNode_InsertL(*ele, &this->task_nodes[task_id].n);
    } else {
        *ele = &this->task_nodes[task_id].n;
    }
}

std::pair<size_t, size_t> MLTScheduler::PopTask(size_t time) {
    if (Zeta_MultiLevelTable_GetSize(&this->mlt) == 0) {
        return { 0, this->tasks_num };
    }

    size_t idxes[ZETA_MultiLevelTable_max_level];
    GetIdxes(&this->mlt, idxes, time);

    void** ele{ Zeta_MultiLevelTable_FindNext(&this->mlt, idxes, TRUE) };

    if (ele == NULL) {
        ele = Zeta_MultiLevelTable_FindNext(&this->mlt, idxes, TRUE);
    }

    ZETA_DebugAssert(ele != NULL);

    MLTTaskNode* task_node{ ZETA_GetStructFromMember(MLTTaskNode, n, *ele) };

    void* nxt_ele{ Zeta_OrdLinkedListNode_GetR(*ele) };

    if (*ele == nxt_ele) {
        Zeta_MultiLevelTable_Erase(&this->mlt, idxes);
    } else {
        Zeta_OrdLinkedListNode_Extract(*ele);
        *ele = nxt_ele;
    }

    return { GetIdx(&this->mlt, idxes), task_node->id };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class RBTScheduler {
public:
    size_t time_capacity;
    size_t tasks_num;

    std::vector<MLTTaskNode> task_nodes;

    std::map<size_t, void*, std::less<size_t>,
             CppStdAllocator<std::pair<const size_t, void*>>>
        rbt;

    RBTScheduler(size_t time_capacity, size_t tasks_num);

    void PushTask(size_t time, size_t task_id);

    std::pair<size_t, size_t> PopTask(size_t time);
};

RBTScheduler::RBTScheduler(size_t time_capacity, size_t tasks_num)
    : time_capacity{ time_capacity },
      tasks_num{ tasks_num },
      rbt{ CppStdAllocator<std::pair<const size_t, void*>>{ 48,
                                                            tasks_num + 4 } } {
    this->task_nodes.resize(this->tasks_num);

    for (size_t task_id{ 0 }; task_id < this->tasks_num; ++task_id) {
        this->task_nodes[task_id].id = task_id;

        Zeta_OrdLinkedListNode_Init(&this->task_nodes[task_id].n);
    }
}

void RBTScheduler::PushTask(size_t time, size_t task_id) {
    auto iter_b{ this->rbt.insert({ time, &this->task_nodes[task_id].n }) };

    auto iter{ iter_b.first };

    if (!iter_b.second) {
        Zeta_OrdLinkedListNode_InsertL(iter->second,
                                       &this->task_nodes[task_id].n);
    }
}

std::pair<size_t, size_t> RBTScheduler::PopTask(size_t time) {
    if (this->rbt.empty()) { return { 0, this->tasks_num }; }

    auto iter{ this->rbt.lower_bound(time) };

    if (iter == this->rbt.end()) { iter = this->rbt.begin(); }

    void* ele = iter->second;
    MLTTaskNode* task_node{ ZETA_GetStructFromMember(MLTTaskNode, n, ele) };

    std::pair<size_t, size_t> ret{ iter->first, task_node->id };

    void* nxt_ele{ Zeta_OrdLinkedListNode_GetR(ele) };

    if (ele == nxt_ele) {
        this->rbt.erase(iter);
    } else {
        Zeta_OrdLinkedListNode_Extract(ele);
        iter->second = nxt_ele;
    }

    return ret;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void main1() {
    ZETA_PrintPos;

    time_t seed = time(NULL);

    printf("seed: %lld\n", seed);

    rand_en.seed(seed);

    MLTScheduler mlt_scheduler{ time_ring, tasks_num };
    RBTScheduler set_scheduler{ time_ring, tasks_num };

    for (size_t task_id{ 0 }; task_id < tasks_num; ++task_id) {
        size_t time = time_generator(rand_en);

        mlt_scheduler.PushTask(time, task_id);
        set_scheduler.PushTask(time, task_id);
    }

    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();

    size_t cur_time = 0;

    for (size_t s{ 0 }; s < 16 * 1024 * 1024; ++s) {
        std::pair<size_t, size_t> mlt_p{ mlt_scheduler.PopTask(cur_time) };
        std::pair<size_t, size_t> set_p{ set_scheduler.PopTask(cur_time) };

        ZETA_DebugAssert(mlt_p.first == set_p.first);
        ZETA_DebugAssert(mlt_p.second == set_p.second);

        size_t nxt_time{ mlt_p.first };
        size_t nxt_task_id{ mlt_p.second };

        // printf("%llu %llu\n", nxt_time, nxt_task_id);

        ZETA_DebugAssert(nxt_task_id != tasks_num);

        size_t time_stride{ time_stride_generator(rand_en) };

        nxt_time = (nxt_time + time_stride) % time_ring;

        mlt_scheduler.PushTask(nxt_time, nxt_task_id);
        set_scheduler.PushTask(nxt_time, nxt_task_id);
    }

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();

    std::cout << "Time difference = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       begin)
                     .count()
              << "[ms]\n";
}

void main2() {
    time_t seed = time(NULL);

    printf("seed: %lld\n", seed);

    rand_en.seed(seed);

    // MLTScheduler scheduler{ time_ring, tasks_num };
    RBTScheduler scheduler{ time_ring, tasks_num };

    for (size_t task_id{ 0 }; task_id < tasks_num; ++task_id) {
        size_t time = time_generator(rand_en);

        scheduler.PushTask(time, task_id);
    }

    size_t cur_time{ 0 };

    size_t hash_code{ 0 };

    size_t scheduling_num{ 16 * 1024 * 1024 };

    std::deque<size_t> time_strides;

    for (size_t s{ 0 }; s < scheduling_num; ++s) {
        time_strides.push_back(time_stride_generator(rand_en));
    }

    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();

    for (size_t s{ 0 }; s < scheduling_num; ++s) {
        std::pair<size_t, size_t> p{ scheduler.PopTask(cur_time) };

        size_t nxt_time{ p.first };
        size_t nxt_task_id{ p.second };

        hash_code = hash_code * 17 + nxt_time;

        ZETA_DebugAssert(nxt_task_id != tasks_num);

        size_t time_stride{ time_strides.front() };
        time_strides.pop_front();

        nxt_time = (nxt_time + time_stride) % time_ring;

        scheduler.PushTask(nxt_time, nxt_task_id);
    }

    ZETA_PrintVar(hash_code);

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();

    std::cout << "Time difference = "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                       begin)
                     .count()
              << "[ms]\n";
}

int main() {
    main2();
    printf("ok\n");
    return 0;
}
