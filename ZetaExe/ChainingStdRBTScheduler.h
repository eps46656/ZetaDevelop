#include <set>

#include "../Zeta/MultiLevelTable.h"
#include "../Zeta/OrdLinkedListNode.h"
#include "CppStdAllocator.h"
#include "ZetaPoolAllocator.h"

class ChainingStdRBTScheduler {
public:
    struct TaskNode {
        size_t id;
        Zeta_OrdLinkedListNode n;
    };

    using rbt_t = std::map<size_t, void*, std::less<size_t>,
                           CppStdAllocator<std::pair<const size_t, void*>>>;

    ChainingStdRBTScheduler(size_t tasks_num);

    void PushTask(size_t priority, size_t task_id);

    std::pair<size_t, size_t> PopTask();

private:
    size_t tasks_num_;

    std::vector<TaskNode> task_nodes_;

    rbt_t rbt_;

    rbt_t::iterator cur_iter_;
};

ChainingStdRBTScheduler::ChainingStdRBTScheduler(size_t tasks_num)
    : tasks_num_{ tasks_num },
      rbt_{ CppStdAllocator<rbt_t::value_type>{ sizeof(rbt_t::value_type),
                                                tasks_num + 4 } } {
    this->task_nodes.resize(this->tasks_num_);

    for (size_t task_id{ 0 }; task_id < this->tasks_num_; ++task_id) {
        this->task_nodes[task_id].id = task_id;

        Zeta_OrdLinkedListNode_Init(&this->task_nodes[task_id].n);
    }

    this->cur_iter_ = this->rbt_.end();
}

void ChainingStdRBTScheduler::PushTask(size_t priority, size_t task_id) {
    auto iter_b{ this->rbt_.insert({ priority, NULL }) };

    auto iter{ iter_b.first };

    if (iter_b.second) {
        iter->second = &this->task_nodes[task_id].n;
    } else {
        Zeta_OrdLinkedListNode_InsertL(iter->second,
                                       &this->task_nodes[task_id].n);
    }
}

std::pair<size_t, size_t> ChainingStdRBTScheduler::PopTask() {
    for (;;) {
        if (this->cur_iter_ == this->rbt_.end()) {
            this->cur_iter_ = this->rbt_.begin();

            if (this->cur_iter_ == this->rbt_.end()) {
                return { 0, this->tasks_num_ };
            }
        }

        if (this->cur_iter_->second != NULL) { break; }

        this->cur_iter_ = this->rbt_.erase(this->cur_iter_);
    }

    void* ele = this->cur_iter_->second;
    TaskNode* task_node{ ZETA_GetStructFromMember(TaskNode, n, ele) };

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
