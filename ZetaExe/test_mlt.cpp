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
size_t tasks_num{ (size_t)1 << 8 };

std::mt19937_64 rand_en;

size_t min_priority_stride{ 1 };
size_t max_priority_stride{ (size_t)1 << 30 };

std::uniform_int_distribution<size_t> time_generator{ 0, time_ring - 1 };

std::uniform_int_distribution<size_t> time_stride_generator{
    min_priority_stride, max_priority_stride
};

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

    size_t cur_time_idxes[ZETA_MultiLevelTable_max_level];
    void** cur_ele;

    MLTScheduler(size_t time_capacity, size_t tasks_num);

    void PushTask(size_t time, size_t task_id);

    std::pair<size_t, size_t> PopTask();
};

MLTScheduler::MLTScheduler(size_t time_capacity, size_t tasks_num)
    : time_capacity{ time_capacity }, tasks_num{ tasks_num } {
    this->task_nodes.resize(this->tasks_num);

    for (size_t task_id{ 0 }; task_id < this->tasks_num; ++task_id) {
        this->task_nodes[task_id].id = task_id;

        Zeta_OrdLinkedListNode_Init(&this->task_nodes[task_id].n);
    }

    size_t needed_capacity{ this->time_capacity };

    int level = 0;

    for (; 1 < needed_capacity;
         needed_capacity /= ZETA_MultiLevelTable_max_branch_num, ++level) {
        this->mlt.branch_nums[level] = ZETA_MultiLevelTable_max_branch_num;
    }

    this->mlt.level = level;

    ZETA_PrintVar(this->mlt.level);

    this->mlt_node_allocator_.size = 512 + 8;
    this->mlt_node_allocator_.max_buffered_ptrs_num = tasks_num * level;

    Zeta_Allocator_Init(&this->mlt_node_allocator);

    ZetaPoolAllocator_DeployAllocator(&this->mlt_node_allocator_,
                                      &this->mlt_node_allocator);

    this->mlt.node_allocator = &this->mlt_node_allocator;

    Zeta_MultiLevelTable_Init(&this->mlt);

    for (int level_i{ 0 }; level_i < level; ++level_i) {
        this->cur_time_idxes[level_i] = 0;
    }

    this->cur_ele = NULL;
}

void MLTScheduler::PushTask(size_t time, size_t task_id) {
    size_t idxes[ZETA_MultiLevelTable_max_level];
    GetIdxes(&this->mlt, idxes, time);

    size_t old_size{ Zeta_MultiLevelTable_GetSize(&this->mlt) };

    void** ele{ Zeta_MultiLevelTable_Insert(&this->mlt, idxes) };

    size_t new_size{ Zeta_MultiLevelTable_GetSize(&this->mlt) };

    if (old_size == new_size && *ele != NULL) {
        Zeta_OrdLinkedListNode_InsertL(*ele, &this->task_nodes[task_id].n);
    } else {
        *ele = &this->task_nodes[task_id].n;
    }
}

std::pair<size_t, size_t> MLTScheduler::PopTask() {
    for (;;) {
        if (this->cur_ele == NULL) {
            this->cur_ele = Zeta_MultiLevelTable_FindNext(
                &this->mlt, this->cur_time_idxes, TRUE);

            if (this->cur_ele == NULL) { return { 0, this->tasks_num }; }
        }

        if (*this->cur_ele != NULL) { break; }

        Zeta_MultiLevelTable_Erase(&this->mlt, this->cur_time_idxes);

        this->cur_ele = Zeta_MultiLevelTable_FindNext(
            &this->mlt, this->cur_time_idxes, TRUE);
    }

    MLTTaskNode* task_node{ ZETA_GetStructFromMember(MLTTaskNode, n,
                                                     *this->cur_ele) };

    void* nxt_ele{ Zeta_OrdLinkedListNode_GetR(*this->cur_ele) };

    if (*this->cur_ele == nxt_ele) {
        *this->cur_ele = NULL;
    } else {
        Zeta_OrdLinkedListNode_Extract(*this->cur_ele);
        *this->cur_ele = nxt_ele;
    }

    return { GetIdx(&this->mlt, this->cur_time_idxes), task_node->id };
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class RBTScheduler {
public:
    using rbt_t = std::map<size_t, void*, std::less<size_t>,
                           CppStdAllocator<std::pair<const size_t, void*>>>;

    size_t time_capacity;
    size_t tasks_num;

    std::vector<MLTTaskNode> task_nodes;

    rbt_t rbt;

    rbt_t::iterator cur_iter;

    RBTScheduler(size_t time_capacity, size_t tasks_num);

    void PushTask(size_t time, size_t task_id);

    std::pair<size_t, size_t> PopTask();
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

    this->cur_iter = this->rbt.end();
}

void RBTScheduler::PushTask(size_t time, size_t task_id) {
    auto iter_b{ this->rbt.insert({ time, NULL }) };

    auto iter{ iter_b.first };

    if (iter_b.second) {
        iter->second = &this->task_nodes[task_id].n;
    } else {
        Zeta_OrdLinkedListNode_InsertL(iter->second,
                                       &this->task_nodes[task_id].n);
    }
}

std::pair<size_t, size_t> RBTScheduler::PopTask() {
    for (;;) {
        if (this->cur_iter == this->rbt.end()) {
            this->cur_iter = this->rbt.begin();

            if (this->cur_iter == this->rbt.end()) {
                return { 0, this->tasks_num };
            }
        }

        if (this->cur_iter->second != NULL) { break; }

        this->cur_iter = this->rbt.erase(this->cur_iter);
    }

    void* ele = this->cur_iter->second;
    MLTTaskNode* task_node{ ZETA_GetStructFromMember(MLTTaskNode, n, ele) };

    std::pair<size_t, size_t> ret{ this->cur_iter->first, task_node->id };

    void* nxt_ele{ Zeta_OrdLinkedListNode_GetR(ele) };

    if (ele == nxt_ele) {
        this->cur_iter->second = NULL;
    } else {
        Zeta_OrdLinkedListNode_Extract(ele);
        this->cur_iter->second = nxt_ele;
    }

    return ret;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void main1() {
    ZETA_PrintPos;

    time_t seed = time(NULL);
    ZETA_PrintVar(seed);

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

    for (size_t s{ 0 }; s < 16 * 1024 * 1024; ++s) {
        std::pair<size_t, size_t> mlt_p{ mlt_scheduler.PopTask() };
        std::pair<size_t, size_t> set_p{ set_scheduler.PopTask() };

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
    ZETA_PrintVar(seed);

    rand_en.seed(seed);

    // MLTScheduler scheduler{ time_ring, tasks_num };
    RBTScheduler scheduler{ time_ring, tasks_num };

    for (size_t task_id{ 0 }; task_id < tasks_num; ++task_id) {
        size_t time = time_stride_generator(rand_en);

        scheduler.PushTask(time, task_id);
    }

    size_t hash_code{ 0 };

    size_t pre_scheduling_num{ 1 * 1024 * 1024 };
    size_t scheduling_num{ 16 * 1024 * 1024 };

    std::deque<size_t> time_strides;

    for (size_t s{ 0 }; s < scheduling_num; ++s) {
        time_strides.push_back(time_stride_generator(rand_en));
    }

    for (size_t s{ 0 }; s < pre_scheduling_num; ++s) {
        std::pair<size_t, size_t> p{ scheduler.PopTask() };

        size_t nxt_time{ p.first };
        size_t nxt_task_id{ p.second };

        hash_code = hash_code * 23 + nxt_time;

        ZETA_DebugAssert(nxt_task_id != tasks_num);

        size_t time_stride{ time_strides.front() };
        time_strides.pop_front();

        nxt_time = (nxt_time + time_stride) % time_ring;

        scheduler.PushTask(nxt_time, nxt_task_id);
    }

    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();

    for (size_t s{ 0 }; s < scheduling_num; ++s) {
        std::pair<size_t, size_t> p{ scheduler.PopTask() };

        size_t nxt_time{ p.first };
        size_t nxt_task_id{ p.second };

        hash_code = hash_code * 23 + nxt_time;

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
