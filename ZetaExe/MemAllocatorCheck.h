#include <map>

bool CheckFullContains(const std::map<size_t, size_t>& allocator_records,
                       const std::map<size_t, size_t>& user_records) {
    for (auto iter{ user_records.begin() }, end{ user_records.end() };
         iter != end; ++iter) {
        auto iter_b{ allocator_records.find(iter->first) };
        if (!iter_b->second || iter_b->second < iter->second) { return false; }
    }

    for (auto iter{ allocator_records.begin() }, end{ allocator_records.end() };
         iter != end; ++iter) {
        auto iter_b{ user_records.find(iter->first) };
        if (!iter_b->second || iter->second < iter_b->second) { return false; }
    }

    return true;
}
