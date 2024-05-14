#pragma once

#include <map>
#include <vector>

#include "../Zeta/MultiLevelTable.h"
#include "../Zeta/OrdLinkedListNode.h"
#include "ZetaPoolAllocator.h"

class ChainingMLTScheduler {
public:
    struct TaskNode {
        size_t id;
        Zeta_OrdLinkedListNode n;
    };

    ChainingMLTScheduler(size_t priority_beg, size_t priority_capacity,
                         size_t tasks_num);

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

    size_t GetIdx_(size_t const* idxes) const;
    void GetIdxes_(size_t* dst, size_t idx) const;
};

ChainingMLTScheduler::ChainingMLTScheduler(size_t priority_beg,
                                           size_t priority_capacity,
                                           size_t tasks_num)
    : priority_capacity_{ priority_capacity }, tasks_num_{ tasks_num } {
    this->task_nodes_.resize(this->tasks_num_);

    for (size_t task_id{ 0 }; task_id < this->tasks_num_; ++task_id) {
        this->task_nodes_[task_id].id = task_id;

        Zeta_OrdLinkedListNode_Init(&this->task_nodes_[task_id].n);
    }

    int level{ 0 };

    for (size_t k{ this->priority_capacity_ }; 1 < k;
         k /= ZETA_MultiLevelTable_max_branch_num, ++level) {
        this->mlt_.branch_nums[level] = ZETA_MultiLevelTable_max_branch_num;
    }

    this->mlt_.level = level;

    ZETA_PrintVar(this->mlt_.level);

    this->mlt_node_allocator_.size = 512 + 8;
    this->mlt_node_allocator_.max_buffered_ptrs_num = this->tasks_num_ * level;

    Zeta_Allocator_Init(&this->mlt_node_allocator_i_);

    ZetaPoolAllocator_DeployAllocator(&this->mlt_node_allocator_,
                                      &this->mlt_node_allocator_i_);

    this->mlt_.node_allocator = &this->mlt_node_allocator_i_;

    Zeta_MultiLevelTable_Init(&this->mlt_);

    this->GetIdxes_(this->cur_priority_idxes_, priority_beg);

    this->cur_ele_ =
        Zeta_MultiLevelTable_Insert(&this->mlt_, this->cur_priority_idxes_);

    *this->cur_ele_ = NULL;
}

void ChainingMLTScheduler::PushTask(size_t priority, size_t task_id) {
    size_t priority_idxes[ZETA_MultiLevelTable_max_level];
    this->GetIdxes_(priority_idxes, priority);

    size_t old_size{ Zeta_MultiLevelTable_GetSize(&this->mlt_) };

    void** ele{ Zeta_MultiLevelTable_Insert(&this->mlt_, priority_idxes) };

    size_t new_size{ Zeta_MultiLevelTable_GetSize(&this->mlt_) };

    if (old_size == new_size && *ele != NULL) {
        Zeta_OrdLinkedListNode_InsertL(*ele, &this->task_nodes_[task_id].n);
    } else {
        *ele = &this->task_nodes_[task_id].n;
    }
}

std::pair<size_t, size_t> ChainingMLTScheduler::PopTask() {
    if (*this->cur_ele_ == NULL) {
        if (Zeta_MultiLevelTable_GetSize(&this->mlt_) == 1) {
            return { 0, this->tasks_num_ };
        }

        Zeta_MultiLevelTable_Erase(&this->mlt_, this->cur_priority_idxes_);

        do {
            this->cur_ele_ = Zeta_MultiLevelTable_FindNext(
                &this->mlt_, this->cur_priority_idxes_, TRUE);
        } while (this->cur_ele_ == NULL);
    }

    TaskNode* task_node{ ZETA_GetStructFromMember(TaskNode, n,
                                                  *this->cur_ele_) };

    void* nxt_ele{ Zeta_OrdLinkedListNode_GetR(*this->cur_ele_) };

    if (*this->cur_ele_ == nxt_ele) {
        *this->cur_ele_ = NULL;
    } else {
        Zeta_OrdLinkedListNode_Extract(*this->cur_ele_);
        *this->cur_ele_ = nxt_ele;
    }

    return { this->GetIdx_(this->cur_priority_idxes_), task_node->id };
}

size_t ChainingMLTScheduler::GetIdx_(size_t const* idxes) const {
    size_t idx{ idxes[0] };

    for (int level_i{ 1 }; level_i < this->mlt_.level; ++level_i) {
        idx = idx * this->mlt_.branch_nums[level_i] + idxes[level_i];
    }

    return idx;
}

void ChainingMLTScheduler::GetIdxes_(size_t* dst, size_t idx) const {
    size_t origin_idx{ idx };

    for (int level_i{ this->mlt_.level - 1 }; 0 <= level_i; --level_i) {
        dst[level_i] = idx % this->mlt_.branch_nums[level_i];
        idx /= this->mlt_.branch_nums[level_i];
    }

    ZETA_DebugAssert(origin_idx == this->GetIdx_(dst));
}
