#include <chrono>
#include <deque>
#include <map>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>

#include "../Zeta/MultiLevelTable.h"
#include "../Zeta/ord_linked_list_node.h"
#include "ZetaPoolallocator.h"
#include "cpp_std_allocator.h"

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

class MLTScheduler {
public:
    struct TaskNode {
        size_t id;
        Zeta_OrdLinkedListNode n;
    };

    MLTScheduler(size_t priority_capacity, size_t tasks_num);

    void PushTask(size_t priority, size_t task_id);

    std::pair<size_t, size_t> PopTask();

private:
    size_t priority_capacity_;
    size_t tasks_num_;

    std::vector<TaskNode> task_nodes_;

    ZetaPoolAllocator mlt_node_allocator_;
    Zeta_Allocator mlt_node_allocator_i_;

    Zeta_MultiLevelTable mlt_;

    size_t cur_priority_idxes_[ZETA_MultiLevelTable_max_level];
    void** cur_ele_;
};

MLTScheduler::MLTScheduler(size_t priority_capacity, size_t tasks_num)
    : priority_capacity_{ priority_capacity }, tasks_num_{ tasks_num } {
    this->task_nodes.resize(this->tasks_num_);

    for (size_t task_id{ 0 }; task_id < this->tasks_num_; ++task_id) {
        this->task_nodes[task_id].id = task_id;

        Zeta_OrdLinkedListNode_Init(&this->task_nodes[task_id].n);
    }

    int level{ 0 };

    for (size_t k{ this->priority_capacity_ }; 1 < k;
         k /= ZETA_MultiLevelTable_max_branch_num, ++level) {
        this->mlt.branch_nums[level] = ZETA_MultiLevelTable_max_branch_num;
    }

    this->mlt.level = level;

    ZETA_PrintVar(this->mlt.level);

    this->mlt_node_allocator_.size = 512 + 8;
    this->mlt_node_allocator_.max_buffered_ptrs_num = this->tasks_num_ * level;

    Zeta_Allocator_Init(&this->mlt_node_allocator_);

    ZetaPoolAllocator_DeployAllocator(&this->mlt_node_allocator_,
                                      &this->mlt_node_allocator_i_);

    this->mlt.node_allocator = &this->mlt_node_allocator_i_;

    Zeta_MultiLevelTable_Init(&this->mlt);

    for (int level_i{ 0 }; level_i < level; ++level_i) {
        this->cur_priority_idxes[level_i] = 0;
    }

    this->cur_ele = NULL;
}

void MLTScheduler::PushTask(size_t priority, size_t task_id) {
    size_t priority_idxes[ZETA_MultiLevelTable_max_level];
    GetIdxes(&this->mlt, priority_idxes, priority);

    size_t old_size{ Zeta_MultiLevelTable_GetSize(&this->mlt) };

    void** ele{ Zeta_MultiLevelTable_Insert(&this->mlt, priority_idxes) };

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
                &this->mlt, this->cur_priority_idxes, TRUE);

            if (this->cur_ele == NULL) { return { 0, this->tasks_num }; }
        }

        if (*this->cur_ele != NULL) { break; }

        Zeta_MultiLevelTable_Erase(&this->mlt, this->cur_priority_idxes);

        this->cur_ele = Zeta_MultiLevelTable_FindNext(
            &this->mlt, this->cur_priority_idxes, TRUE);
    }

    TaskNode* task_node{ ZETA_MemberToStruct(TaskNode, n, *this->cur_ele) };

    void* nxt_ele{ Zeta_OrdLinkedListNode_GetR(*this->cur_ele) };

    if (*this->cur_ele == nxt_ele) {
        *this->cur_ele = NULL;
    } else {
        Zeta_OrdLinkedListNode_Extract(*this->cur_ele);
        *this->cur_ele = nxt_ele;
    }

    return { GetIdx(&this->mlt, this->cur_priority_idxes), task_node->id };
}
