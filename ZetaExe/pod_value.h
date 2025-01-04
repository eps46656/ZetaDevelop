#pragma once

#include <utils.h>

#include <iostream>
#include <unordered_map>

#include "cmp_utils.h"
#include "hash_utils.h"
#include "random.h"

struct PODValue {
    static constexpr size_t width{ 7 };
    unsigned char pre_padding[8];
    unsigned char data[width];
    unsigned char post_padding[8];
};

std::ostream& operator<<(std::ostream& os, PODValue const& val) {
    char cs[]{ "0123456789ABCDEF" };

    for (size_t i{ PODValue::width }; 0 < i--;) {
        os << cs[val.data[i] / 16] << cs[val.data[i] % 16];
    }

    return os;
}

template <>
struct HashCore<PODValue> {
    unsigned long long operator()(void const* context, PODValue const& x,
                                  unsigned long long salt) {
        ZETA_Unused(context);

        return Zeta_MemHash(x.data, PODValue::width, salt);
    }
};

namespace std {

template <>
struct hash<PODValue> {
    unsigned long long operator()(PODValue const& x) const {
        return Hash(NULL, x, PODValue::width);
    }
};

}  // namespace std

template <>
struct CompareCore<PODValue, PODValue> {
    int operator()(void const* context, PODValue const& x,
                   PODValue const& y) const {
        ZETA_Unused(context);

        for (size_t i{ 0 }; i < PODValue::width; ++i) {
            switch (ZETA_ThreeWayCompare(x.data[i], y.data[i])) {
                case -1: return -1;
                case 1: return 1;
            }
        }

        return 0;
    }
};

bool_t operator==(PODValue const& x, PODValue const& y) {
    return Compare(NULL, x, y) == 0;
}

bool_t operator!=(PODValue const& x, PODValue const& y) {
    return Compare(NULL, x, y) != 0;
}

bool_t operator<(PODValue const& x, PODValue const& y) {
    return Compare(NULL, x, y) < 0;
}

bool_t operator<=(PODValue const& x, PODValue const& y) {
    return Compare(NULL, x, y) <= 0;
}

bool_t operator>(PODValue const& x, PODValue const& y) {
    return Compare(NULL, x, y) > 0;
}

bool_t operator>=(PODValue const& x, PODValue const& y) {
    return Compare(NULL, x, y) >= 0;
}

template <>
struct GetRandomCore<PODValue> {
    PODValue operator()() const {
        PODValue ret;

        for (size_t i{ 0 }; i < PODValue::width; ++i) {
            ret.data[i] = GetRandomInt<unsigned char>(0, 255);
        }

        return ret;
    }
};
