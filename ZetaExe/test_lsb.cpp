#include <random>
#include <unordered_map>

#include "../Zeta/utils.h"

std::mt19937_64 en;
std::uniform_int_distribution<size_t> size_generator{ 0,
                                                      ZETA_RangeMaxOf(size_t) };
std::uniform_int_distribution<size_t> val_generator{ 0,
                                                     ZETA_RangeMaxOf(size_t) };

int GetLSB(size_t x) {
    if (x == 0) { return -1; }

    int ret = 0;

    for (; x % 2 == 0; x /= 2) { ++ret; }

    return ret;
}

int GetMSB(size_t x) {
    int ret = -1;

    for (; 0 < x; x /= 2) { ++ret; }

    return ret;
}

void main1() {
    time_t seed = time(NULL);

    ZETA_PrintVar(seed);

    en.seed(seed);

    for (size_t test_i = 0; test_i < 1000 * 1000; ++test_i) {
        size_t val = val_generator(en);
        ZETA_DebugAssert(Zeta_GetLSB(val) == GetLSB(val));
        ZETA_DebugAssert(Zeta_GetMSB(val) == GetMSB(val));
    }
}

int main() {
    main1();

    ZETA_PrintVar(ZETA_IsDebug);

    std::cout << "ok\n";

    return 0;
}
