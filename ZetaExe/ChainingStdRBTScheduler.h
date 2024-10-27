#pragma once

#include <map>
#include <vector>

#include "../Zeta/ord_linked_list_node.h"
#include "cpp_std_allocator.h"

class ChainingStdRBTScheduler {
public:
    struct TaskNode {
        size_t id;
        Zeta_OrdLinkedListNode n;
    };

    using rbt_t = std::map<size_t, void*, std::less<size_t>,
                           CppStdAllocator<std::pair<const size_t, void*>>>;

    ChainingStdRBTScheduler(size_t priority_beg, size_t tasks_num);

    void PushTask(size_t priority, size_t task_id);

    std::pair<size_t, size_t> PopTask();

    size_t GetMemUsage() const;

private:
    size_t tasks_num_;

    std::vector<TaskNode> task_nodes_;

    rbt_t rbt_;

    rbt_t::iterator cur_iter_;
};

ChainingStdRBTScheduler::ChainingStdRBTScheduler(size_t priority_beg,
                                                 size_t tasks_num)
    : tasks_num_{ tasks_num },
      rbt_{ CppStdAllocator<rbt_t::value_type>{ sizeof(rbt_t::value_type),
                                                tasks_num + 4 } } {
    this->task_nodes_.resize(this->tasks_num_);

    for (size_t task_id{ 0 }; task_id < this->tasks_num_; ++task_id) {
        this->task_nodes_[task_id].id = task_id;

        Zeta_OrdLinkedListNode_Init(&this->task_nodes_[task_id].n);
    }

    this->cur_iter_ = this->rbt_.insert({ priority_beg, NULL }).first;
}

void ChainingStdRBTScheduler::PushTask(size_t priority, size_t task_id) {
    auto iter_b{ this->rbt_.insert({ priority, NULL }) };

    auto iter{ iter_b.first };

    if (iter_b.second || iter->second == NULL) {
        iter->second = &this->task_nodes_[task_id].n;
    } else {
        Zeta_OrdLinkedListNode_InsertL(iter->second,
                                       &this->task_nodes_[task_id].n);
    }
}

std::pair<size_t, size_t> ChainingStdRBTScheduler::PopTask() {
    if (this->cur_iter_->second == NULL) {
        if (this->rbt_.size() == 1) { return { 0, this->tasks_num_ }; }

        this->cur_iter_ = this->rbt_.erase(this->cur_iter_);

        if (this->cur_iter_ == this->rbt_.end()) {
            this->cur_iter_ = this->rbt_.begin();
        }
    }

    void* ele = this->cur_iter_->second;
    TaskNode* task_node{ ZETA_MemberToStruct(TaskNode, n, ele) };

    std::pair<size_t, size_t> ret{ this->cur_iter_->first, task_node->id };

    void* nxt_ele{ Zeta_OrdLinkedListNode_GetR(ele) };

    if (ele == nxt_ele) {
        this->cur_iter_->second = NULL;
    } else {
        Zeta_OrdLinkedListNode_Extract(ele);
        this->cur_iter_->second = nxt_ele;
    }

    return ret;
}

size_t ChainingStdRBTScheduler::GetMemUsage() const {
    return sizeof(TaskNode) * this->task_nodes_.size() +
           this->rbt_.get_allocator().usage();
}
