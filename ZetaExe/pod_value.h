#pragma once

#include <utils.h>

#include <iostream>
#include <unordered_map>

#include "random.h"

struct PODValue {
    static constexpr size_t width{ 7 };
    unsigned char pre_padding[8];
    unsigned char data[width];
    unsigned char post_padding[8];
};

std::ostream& operator<<(std::ostream& os, const PODValue& val) {
    char cs[]{ "0123456789ABCDEF" };

    for (size_t i{ PODValue::width }; 0 < i--;) {
        os << cs[val.data[i] / 16] << cs[val.data[i] % 16];
    }

    return os;
}

unsigned long long PODValueHash(const PODValue& x) {
    return Zeta_MemHash(x.data, PODValue::width);
}

namespace std {

template <>
struct hash<PODValue> {
    unsigned long long operator()(const PODValue& x) const {
        return Zeta_MemHash(x.data, PODValue::width);
    }
};

}  // namespace std

int PODValueCompare(const PODValue& x, const PODValue& y) {
    for (size_t i{ 0 }; i < PODValue::width; ++i) {
        switch (ZETA_ThreeWayCompare(x.data[i], y.data[i])) {
            case -1: return -1;
            case 1: return 1;
        }
    }

    return 0;
}

bool operator==(const PODValue& x, const PODValue& y) {
    return PODValueCompare(x, y) == 0;
}

bool operator!=(const PODValue& x, const PODValue& y) {
    return PODValueCompare(x, y) != 0;
}

bool operator<(const PODValue& x, const PODValue& y) {
    return PODValueCompare(x, y) < 0;
}

bool operator<=(const PODValue& x, const PODValue& y) {
    return PODValueCompare(x, y) <= 0;
}

bool operator>(const PODValue& x, const PODValue& y) {
    return PODValueCompare(x, y) > 0;
}

bool operator>=(const PODValue& x, const PODValue& y) {
    return PODValueCompare(x, y) >= 0;
}

template <>
PODValue GetRandom<PODValue>() {
    PODValue ret;

    for (size_t i{ 0 }; i < PODValue::width; ++i) {
        ret.data[i] = GetRandomInt<unsigned char>(0, 255);
    }

    return ret;
}
