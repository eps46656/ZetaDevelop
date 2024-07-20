#pragma once

#include <algorithm>
#include <unordered_map>

inline void CheckRecords(
    std::unordered_map<unsigned long long, unsigned long long> const&
        allocator_records,
    std::unordered_map<unsigned long long, unsigned long long> const&
        user_records) {
    for (auto iter{ user_records.begin() }, end{ user_records.end() };
         iter != end; ++iter) {
        auto iter_b{ allocator_records.find(iter->first) };

        ZETA_DebugAssert(iter_b != allocator_records.end());
        // hallucination: user uses unkonwn memory

        ZETA_DebugAssert(iter->second <= iter_b->second);
        // overflow: user uses more memory than allocated
    }

    for (auto iter{ allocator_records.begin() }, end{ allocator_records.end() };
         iter != end; ++iter) {
        auto iter_b{ user_records.find(iter->first) };

        ZETA_DebugAssert(iter_b != user_records.end());
        // leak: user misses allocated memory

        ZETA_DebugAssert(iter_b->second <= iter->second);
        // overflow: user uses more memory than allocated
    }
}

inline void CheckNoOverlap(
    std::unordered_map<unsigned long long, unsigned long long> const& records) {
    if (records.empty()) { return; }

    std::vector<std::pair<unsigned long long, unsigned long long>> records_vec{
        records.begin(), records.end()
    };

    std::sort(records_vec.begin(), records_vec.end());

    auto iter{ records_vec.begin() };
    auto nxt_iter{ std::next(iter) };
    auto end{ records_vec.end() };

    while (nxt_iter != end) {
        ZETA_DebugAssert(iter->first + iter->second <= nxt_iter->first);

        iter = nxt_iter;
        ++nxt_iter;
    }
}
