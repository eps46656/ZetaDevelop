#include <debugger.h>

#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

#include "dynamic_search_table.h"
#include "naive_search_table.h"
#include "random.h"
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
using DST = DynamicSearchTable<int, std::less<int>>;

int GetRandomVal() { return GetRandomInt<int, int>(0, INT_MAX - 1); }

int SyncLowerBound(NST& nst, DST& dst, int val) {
    const int* nst_val{ nst.LowerBound(val) };

    const int* dst_val{ dst.LowerBound(val) };
    dst.Sanitize();

    ZETA_DebugAssert((nst_val == nullptr) == (dst_val == nullptr));

    if (nst_val == nullptr) { return INT_MAX; }

    ZETA_DebugAssert(*nst_val == *dst_val);

    return *nst_val;
}

int SyncUpperBound(NST& nst, DST& dst, int val) {
    const int* nst_val{ nst.UpperBound(val) };

    const int* dst_val{ dst.UpperBound(val) };
    dst.Sanitize();

    ZETA_DebugAssert((nst_val == nullptr) == (dst_val == nullptr));

    if (nst_val == nullptr) { return INT_MAX; }

    ZETA_DebugAssert(*nst_val == *dst_val);

    return *nst_val;
}

void SyncInsert(NST& nst, DST& dst, int val) {
    nst.Insert(val);

    dst.Insert(val);
    dst.Sanitize();
}

void SyncErase(NST& nst, DST& dst, int val) {
    nst.Erase(val);

    dst.Erase(val);
    dst.Sanitize();
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

    RandomEngine().seed(seed);

    size_t size{ 1024 * 16 };

    std::vector<int> vec;
    vec.resize(size);

    for (int& val : vec) { val = GetRandomVal(); }

    NST nst{ vec };
    DST dst{ vec };

    dst.Sanitize();

    for (int test_i{ 0 }; test_i < 64; ++test_i) {
        ZETA_PrintVar(test_i);
        ZETA_PrintVar(nst.size());

        SyncLowerBound(nst, dst, GetRandomVal());
        SyncUpperBound(nst, dst, GetRandomVal());

        for (int test_j{ 0 }; test_j < 256; ++test_j) {
            SyncInsert(nst, dst, GetRandomVal());
        }

        SyncUpperBound(nst, dst, GetRandomVal());

        for (int test_j{ 0 }; test_j < 16; ++test_j) {
            int val{ SyncLowerBound(nst, dst, GetRandomVal()) };
            if (val < INT_MAX) { SyncErase(nst, dst, val); }
        }

        for (int test_j{ 0 }; test_j < 16; ++test_j) {
            int val{ SyncUpperBound(nst, dst, GetRandomVal()) };
            if (val < INT_MAX) { SyncErase(nst, dst, val); }
        }
    }
}

int main() {
    printf("main start\n");

    ZETA_PrintCurPos;
    unsigned long long beg_time{ GetTime() };

    main1();

    unsigned long long end_time{ GetTime() };

    printf("ok\a\n");

    std::cout << "Time difference = " << end_time - beg_time << "[ms]\n";

    return 0;
}
