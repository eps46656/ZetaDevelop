#include <debugger.h>

#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

#include "dynamic_search_table.h"
#include "naive_search_table.h"
#include "random.h"
#include "static_search_table.h"
#include "timer.h"

#define PRINT_VAR(var)                                                 \
    std::cout << __FILE__ << ":" << __LINE__ << "\t\t" << #var << ": " \
              << (var) << "\n";

void Print(const int* x) {
    if (x == nullptr) {
        PRINT_VAR("nullptr");
    } else {
        PRINT_VAR(*x);
    }
}

using NST = NaiveSearchTable<int, std::less<int>>;
using SST = StaticSearchTable<int, std::less<int>>;
using DST = DynamicSearchTable<int, std::less<int>>;

int GetRandomVal() { return GetRandomInt<int, int>(0, INT_MAX - 1); }

template <typename STA, typename STB>
int SyncLowerBound(STA& sta, STB& stb, int val) {
    assert(sta.size() == stb.size());

    const int* sta_val{ sta.LowerBound(val) };
    sta.Sanitize();

    const int* stb_val{ stb.LowerBound(val) };
    stb.Sanitize();

    ZETA_DebugAssert((sta_val == nullptr) == (stb_val == nullptr));

    if (sta_val == nullptr) { return INT_MAX; }

    ZETA_DebugAssert(*sta_val == *stb_val);

    return *sta_val;
}

template <typename STA, typename STB>
int SyncUpperBound(STA& sta, STB& stb, int val) {
    assert(sta.size() == stb.size());

    const int* sta_val{ sta.UpperBound(val) };
    sta.Sanitize();

    const int* stb_val{ stb.UpperBound(val) };
    stb.Sanitize();

    ZETA_DebugAssert((sta_val == nullptr) == (stb_val == nullptr));

    if (sta_val == nullptr) { return INT_MAX; }

    ZETA_DebugAssert(*sta_val == *stb_val);

    return *sta_val;
}

template <typename STA, typename STB>
void SyncInsert(STA& sta, STB& stb, int val) {
    assert(sta.size() == stb.size());

    sta.Insert(val);
    sta.Sanitize();

    stb.Insert(val);
    stb.Sanitize();
}

template <typename STA, typename STB>
void SyncErase(STA& sta, STB& stb, int val) {
    assert(sta.size() == stb.size());

    sta.Erase(val);
    sta.Sanitize();

    stb.Erase(val);
    stb.Sanitize();
}

void main1() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1729615114;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    SetRandomSeed(seed);

    size_t size{ 1024 * 16 };

    std::vector<int> vec;
    vec.resize(size);

    for (int& val : vec) { val = GetRandomVal(); }

    NST sta{ std::less<int>{} };
    DST stb{ std::less<int>{} };

    stb.Sanitize();

    for (int test_i{ 0 }; test_i < 64; ++test_i) {
        ZETA_PrintVar(test_i);
        ZETA_PrintVar(sta.size());

        SyncLowerBound(sta, stb, GetRandomVal());
        SyncUpperBound(sta, stb, GetRandomVal());

        for (int test_j{ 0 }; test_j < 256; ++test_j) {
            SyncInsert(sta, stb, GetRandomVal());
        }

        SyncUpperBound(sta, stb, GetRandomVal());

        for (int test_j{ 0 }; test_j < 16; ++test_j) {
            int val{ SyncLowerBound(sta, stb, GetRandomVal()) };
            if (val < INT_MAX) { SyncErase(sta, stb, val); }
        }

        for (int test_j{ 0 }; test_j < 16; ++test_j) {
            int val{ SyncUpperBound(sta, stb, GetRandomVal()) };
            if (val < INT_MAX) { SyncErase(sta, stb, val); }
        }
    }
}

void main2() {
    unsigned random_seed = time(NULL);
    unsigned fixed_seed = 1735624351;

    unsigned seed = random_seed;
    // unsigned seed = fixed_seed;

    ZETA_PrintCurPos;

    ZETA_PrintVar(random_seed);
    ZETA_PrintVar(fixed_seed);
    ZETA_PrintVar(seed);

    SetRandomSeed(seed);

    size_t size{ 1024 * 1024 * 16 };

    std::vector<int> vec;
    vec.resize(size);

    for (int& val : vec) { val = GetRandomVal(); }

    NST sta{ std::less<int>{} };
    SST stb{ std::less<int>{} };

    sta.Sanitize();
    stb.Sanitize();

    for (int test_i{ 0 }; test_i < 64; ++test_i) {
        ZETA_PrintVar(test_i);
        ZETA_PrintVar(sta.size());

        SyncLowerBound(sta, stb, GetRandomVal());
        SyncUpperBound(sta, stb, GetRandomVal());

        SyncUpperBound(sta, stb, GetRandomVal());
    }
}

int main() {
    printf("main start\n");

    ZETA_PrintCurPos;
    unsigned long long beg_time{ GetTime() };

    main2();

    unsigned long long end_time{ GetTime() };

    printf("ok\a\n");

    std::cout << "Time difference = " << end_time - beg_time << "[ms]\n";

    return 0;
}
