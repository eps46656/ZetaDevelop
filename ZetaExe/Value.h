#pragma once

struct Value {
    static constexpr size_t width{ 7 };
    unsigned char pre_padding[8];
    unsigned char data[width];
    unsigned char post_padding[8];
};

int ValueCompare(const Value& x, const Value& y);

unsigned long long ValueHash(const Value& x);

bool operator==(const Value& x, const Value& y);
bool operator!=(const Value& x, const Value& y);

bool operator<(const Value& x, const Value& y);
bool operator<=(const Value& x, const Value& y);
bool operator>(const Value& x, const Value& y);
bool operator>=(const Value& x, const Value& y);
