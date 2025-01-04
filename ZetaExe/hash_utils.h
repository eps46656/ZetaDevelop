#pragma once

#include <define.h>

#include <utility>

template <typename Value>
struct HashCore {
    unsigned long long operator()(void const* context, const Value& a,
                                  unsigned long long salt) const {
        ZETA_Unused(context);
        return std::hash<Value>{}(a) * 23 + salt;
    }
};

template <typename Value>
unsigned long long Hash(void const* context, const Value& a,
                        unsigned long long salt) {
    return HashCore<Value>{}(context, a, salt);
}

template <typename Value>
unsigned long long ZetaHash(void const* context, void const* a,
                            unsigned long long salt) {
    return Hash(context, *(Value const*)a, salt);
}
