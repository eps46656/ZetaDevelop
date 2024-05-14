#pragma once

#include <set>

#include "CppStdAllocator.h"

class NaiveStdRBTScheduler {
public:
    using rbt_t = std::set<std::pair<size_t, size_t>,
                           std::less<std::pair<size_t, size_t>>,
                           CppStdAllocator<std::pair<size_t, size_t>>>;

    NaiveStdRBTScheduler(size_t priority_beg, size_t tasks_num);

    void PushTask(size_t priority, size_t task_id);

    std::pair<size_t, size_t> PopTask();

private:
    size_t tasks_num_;

    rbt_t rbt_;

    size_t priority_last_;
};

NaiveStdRBTScheduler::NaiveStdRBTScheduler(size_t priority_beg,
                                           size_t tasks_num)
    : tasks_num_{ tasks_num },
      rbt_{ CppStdAllocator<rbt_t::value_type>{ sizeof(rbt_t::value_type),
                                                tasks_num + 4 } },
      priority_last_{ priority_beg } {}

void NaiveStdRBTScheduler::PushTask(size_t priority, size_t task_id) {
    this->rbt_.insert({ priority, task_id });
}

std::pair<size_t, size_t> NaiveStdRBTScheduler::PopTask() {
    if (this->rbt_.empty()) { return { 0, this->tasks_num_ }; }

    auto iter{ this->rbt_.lower_bound({ this->priority_last_, 0 }) };
    if (iter == this->rbt_.end()) { iter = this->rbt_.begin(); }

    this->priority_last_ = iter->first;

    std::pair<size_t, size_t> ret{ *iter };

    this->rbt_.erase(iter);

    return ret;
}
