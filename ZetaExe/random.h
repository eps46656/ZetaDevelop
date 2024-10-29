#pragma once

#include <define.h>

#include <random>

std::mt19937_64& RandomEngine() {
    static std::mt19937_64 en;
    return en;
}

void SetRandomSeed(unsigned seed) { RandomEngine().seed(seed); }

long long GetRandomInt(long long lb, long long rb) {
    static std::uniform_int_distribution<long long> ll_generator{
        0, ZETA_RangeMaxOf(long long)
    };

    return ll_generator(RandomEngine()) % (rb - lb + 1) + lb;
}
