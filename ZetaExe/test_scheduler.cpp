#include <chrono>
#include <deque>
#include <map>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>

#include "../Zeta/MultiLevelTable.h"
#include "../Zeta/OrdLinkedListNode.h"
#include "ChainingMLTScheduler.h"
#include "ChainingStdRBTScheduler.h"
#include "NaiveStdRBTScheduler.h"

size_t time_ring{ (size_t)1 << 32 };
size_t tasks_num{ (size_t)1 << 6 };

std::mt19937_64 rand_en;

size_t min_priority_stride{ 1 };
size_t max_priority_stride{ (size_t)1 << 30 };

size_t priority_capacity{ max_priority_stride * 4 };

std::uniform_int_distribution<size_t> time_generator{ 0, time_ring - 1 };

std::uniform_int_distribution<size_t> time_stride_generator{
    min_priority_stride, max_priority_stride
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void main1() {
    ZETA_PrintPos;

    time_t seed = time(NULL);
    ZETA_PrintVar(seed);

    rand_en.seed(seed);

    NaiveStdRBTScheduler scheduler_a{ 0, tasks_num };

    // ChainingStdRBTScheduler scheduler_b{ 0, tasks_num };
    ChainingMLTScheduler scheduler_b{ 0, time_ring, tasks_num };

    ZETA_PrintPos;

    for (size_t task_id{ 0 }; task_id < tasks_num; ++task_id) {
        size_t time = time_generator(rand_en);

        scheduler_a.PushTask(time, task_id);
        scheduler_b.PushTask(time, task_id);
    }

    std::chrono::steady_clock::time_point begin =
        std::chrono::steady_clock::now();

    for (size_t s{ 0 }; s < 16 * 1024 * 1024; ++s) {
        std::pair<size_t, size_t> mlt_p{ scheduler_a.PopTask() };
        std::pair<size_t, size_t> set_p{ scheduler_b.PopTask() };

        ZETA_DebugAssert(mlt_p.first == set_p.first);
        // ZETA_DebugAssert(mlt_p.second == set_p.second);

        size_t nxt_time{ mlt_p.first };
        size_t nxt_task_id{ mlt_p.second };

        // printf("%llu %llu\n", nxt_time, nxt_task_id);

        ZETA_DebugAssert(nxt_task_id != tasks_num);

        size_t time_stride{ time_stride_generator(rand_en) };

        nxt_time = (nxt_time + time_stride) % time_ring;

        scheduler_a.PushTask(nxt_time, nxt_task_id);
        scheduler_b.PushTask(nxt_time, nxt_task_id);
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
    // time_t seed = 1715684680;
    ZETA_PrintVar(seed);

    rand_en.seed(seed);

    // NaiveStdRBTScheduler scheduler{ 0, tasks_num };
    // ChainingStdRBTScheduler scheduler{ 0, tasks_num };
    ChainingMLTScheduler scheduler{ 0, time_ring, tasks_num };

    for (size_t task_id{ 0 }; task_id < tasks_num; ++task_id) {
        size_t time = time_stride_generator(rand_en);

        scheduler.PushTask(time, task_id);
    }

    size_t hash_code{ 0 };

    size_t pre_scheduling_num{ 1 * 1024 * 1024 };
    size_t scheduling_num{ 16 * 1024 * 1024 };

    std::deque<size_t> time_strides;

    for (size_t s{ 0 }; s < pre_scheduling_num + scheduling_num; ++s) {
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
