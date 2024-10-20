#pragma once

#include <unordered_map>

struct Value {
    static constexpr size_t width{ 7 };
    unsigned char pre_padding[8];
    unsigned char data[width];
    unsigned char post_padding[8];
};

unsigned long long ValueHash(const Value& x) {
    std::hash<unsigned char> hash;

    unsigned long long ret{ 0 };

    for (size_t i{ 0 }; i < Value::width; ++i) { ret ^= hash(x.data[i]); }

    return ret;
}

int ValueCompare(const Value& x, const Value& y) {
    for (size_t i{ 0 }; i < Value::width; ++i) {
        if (x.data[i] < y.data[i]) { return -1; }
        if (y.data[i] < x.data[i]) { return 1; }
    }

    return 0;
}

bool operator==(const Value& x, const Value& y) {
    return ValueCompare(x, y) == 0;
}

bool operator!=(const Value& x, const Value& y) {
    return ValueCompare(x, y) != 0;
}

bool operator<(const Value& x, const Value& y) {
    return ValueCompare(x, y) < 0;
}

bool operator<=(const Value& x, const Value& y) {
    return ValueCompare(x, y) <= 0;
}

bool operator>(const Value& x, const Value& y) {
    return ValueCompare(x, y) > 0;
}

bool operator>=(const Value& x, const Value& y) {
    return ValueCompare(x, y) >= 0;
}
