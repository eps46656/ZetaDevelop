#pragma once

#include <define.h>

#include <utility>

#include "cmp_utils.h"
#include "hash_utils.h"
#include "random.h"

template <typename First, typename Second>
struct Pair {
    First first;
    Second second;
};

template <typename First, typename Second>
struct HashCore<Pair<First, Second>> {
    unsigned long long operator()(void const* context,
                                  Pair<First, Second> const& pair,
                                  unsigned long long salt) const {
        ZETA_Unused(context);
        return Hash(NULL, pair.first, salt) * 32 +
               Hash(NULL, pair.second, salt);
    }
};

namespace std {

template <typename First, typename Second>
struct hash<Pair<First, Second>> {
    unsigned long long operator()(Pair<First, Second> const& pair) const {
        return Hash(NULL, pair, 0);
    }
};

}  // namespace std

template <typename FirstA, typename SecondA, typename FirstB, typename SecondB>
struct CompareCore<Pair<FirstA, SecondA>, Pair<FirstB, SecondB>> {
    unsigned long long operator()(void const* context,
                                  Pair<FirstA, SecondA> const& a,
                                  Pair<FirstB, SecondB> const& b) const {
        ZETA_Unused(context);

        int cmp = Compare(NULL, a.first, b.first);

        return cmp == 0 ? Compare(NULL, a.second, b.second) : cmp;
    }
};

template <typename FirstA, typename SecondA, typename FirstB, typename SecondB>
bool_t operator==(Pair<FirstA, SecondA> const& a,
                  Pair<FirstB, SecondB> const& b) {
    return Compare(NULL, a, b) == 0;
}

template <typename FirstA, typename SecondA, typename FirstB, typename SecondB>
bool_t operator!=(Pair<FirstA, SecondA> const& a,
                  Pair<FirstB, SecondB> const& b) {
    return Compare(NULL, a, b) != 0;
}

template <typename FirstA, typename SecondA, typename FirstB, typename SecondB>
bool_t operator<(Pair<FirstA, SecondA> const& a,
                 Pair<FirstB, SecondB> const& b) {
    return Compare(NULL, a, b) < 0;
}

template <typename FirstA, typename SecondA, typename FirstB, typename SecondB>
bool_t operator>(Pair<FirstA, SecondA> const& a,
                 Pair<FirstB, SecondB> const& b) {
    return Compare(NULL, a, b) > 0;
}

template <typename FirstA, typename SecondA, typename FirstB, typename SecondB>
bool_t operator<=(Pair<FirstA, SecondA> const& a,
                  Pair<FirstB, SecondB> const& b) {
    return Compare(NULL, a, b) <= 0;
}

template <typename FirstA, typename SecondA, typename FirstB, typename SecondB>
bool_t operator>=(Pair<FirstA, SecondA> const& a,
                  Pair<FirstB, SecondB> const& b) {
    return Compare(NULL, a, b) >= 0;
}

template <typename First, typename Second>
struct GetRandomCore<Pair<First, Second>> {
    Pair<First, Second> operator()() const {
        return { GetRandom<First>(), GetRandom<Second>() };
    }
};
