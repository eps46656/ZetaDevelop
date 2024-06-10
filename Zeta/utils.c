#include "utils.h"

void Zeta_MemCopy(void* dst_, void const* src_, size_t size) {
    unsigned char* dst = dst_;
    unsigned char const* src = src_;

    if (dst == src) { return; }

    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    if (src < dst && dst < src + size) {
        for (size_t i = size; 0 < i--;) { dst[i] = src[i]; }
    } else {
        for (size_t i = 0; i < size; ++i) { dst[i] = src[i]; }
    }
}

void Zeta_MemSwap(void* x_, void* y_, size_t size) {
    unsigned char* x = x_;
    unsigned char* y = y_;

    if (x == y) { return; }

    ZETA_DebugAssert(x != NULL);
    ZETA_DebugAssert(y != NULL);

    for (size_t i = 0; i < size; ++i) { ZETA_Swap(x[i], y[i]); }
}

void* Zeta_MemRotate(void* beg_, void* mid_, void* end_) {
    unsigned char* beg = beg_;
    unsigned char* mid = mid_;
    unsigned char* end = end_;

    ZETA_DebugAssert(beg != NULL);
    ZETA_DebugAssert(mid != NULL);
    ZETA_DebugAssert(end != NULL);

    ZETA_DebugAssert(beg <= mid);
    ZETA_DebugAssert(mid <= end);

    if (beg == mid) { return end; }
    if (mid == end) { return beg; }

    ptrdiff_t a = mid - beg;
    ptrdiff_t b = end - mid;

    unsigned char* ret = beg + b;

    while (0 < a && 0 < b) {
        ptrdiff_t r = b % a;

        a -= r;
        b = r;

        for (; mid != end; ++beg, ++mid) { ZETA_Swap(*beg, *mid); }

        mid -= r;
    }

    return ret;
}

u128_t Zeta_ReadLittleEndian(byte_t const* data, unsigned int length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(length <= sizeof(u128_t));

    u128_t ret = 0;

    for (unsigned int i = length; 0 < i--;) { ret = ret * 256 + data[i]; }

    return ret;
}

u128_t Zeta_WriteLittleEndian(byte_t* dst, u128_t val, unsigned int length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned int i = 0; i < length; ++i) {
        dst[i] = val % 256;
        val /= 256;
    }

    return val;
}

u128_t Zeta_ReadBigEndian(byte_t const* data, unsigned int length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(length <= sizeof(u128_t));

    u128_t ret = 0;

    for (unsigned int i = 0; i < length; ++i) { ret = ret * 256 + data[i]; }

    return ret;
}

u128_t Zeta_WriteBigEndian(byte_t* dst, u128_t val, unsigned int length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned int i = length; 0 < i--;) {
        dst[i] = val % 256;
        val /= 256;
    }

    return val;
}

size_t Zeta_SimpleRandom(size_t x) {
    size_t mask = 0xFFFF;  // 2^16 - 1
    size_t a = 25214903917;
    size_t c = 11;

    return ((x * a + c) >> 16) & mask;
}

size_t Zeta_SimpleRandomRotate(size_t* x) {
    size_t m = 0x80000000;
    size_t a = 0x41C64E6D;
    size_t c = 3039;

    return (*x = (*x * a + c) % m) / 0x10000;
}

size_t Zeta_SimpleHash(size_t x) {
    x = (x ^ (x >> 30)) * (size_t)0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * (size_t)0x94d049bb133111eb;
    x = x ^ (x >> 31);
    return x;
}

size_t Zeta_GetGCD(size_t x, size_t y) {
    if (x == 0 && y == 0) { return 1; }

    for (;;) {
        if (x == 0) { return y; }
        y %= x;
        if (y == 0) { return x; }
        x %= y;
    }
}

size_t Zeta_GetLCM(size_t x, size_t y) { return x / Zeta_GetGCD(x, y) * y; }

size_t Zeta_GetPower(size_t base, size_t exp) {
    size_t ret = 1;

    for (; 0 < exp; exp /= 2) {
        if (exp % 2 == 1) { ret *= base; }
        base *= base;
    }

    return ret;
}

size_t Zeta_GetMulMod(size_t x, size_t y, size_t mod) {
    ZETA_DebugAssert(0 < mod);
    ZETA_DebugAssert(mod <= ZETA_GetMaxMod(size_t));

    x %= mod;
    y %= mod;

    if (x == 0 || y <= ZETA_GetRangeMax(size_t) / x) { return (x * y) % mod; }

    if (x < y) { ZETA_Swap(x, y); }

    size_t ret = 0;

    for (; 0 < y; y /= 2) {
        if (y % 2 == 1) { ret = (ret + x) % mod; }
        x = (x + x) % mod;
    }

    return ret;
}

size_t Zeta_GetPowerMod(size_t base, size_t exp, size_t mod) {
    ZETA_DebugAssert(0 < mod);
    ZETA_DebugAssert(mod <= ZETA_GetMaxMod(size_t));

    base %= mod;

    if (base == 0) { return 0; }

    size_t ret = 1;

    for (; 0 < exp; exp /= 2) {
        if (exp % 2 == 1) { ret = Zeta_GetMulMod(ret, base, mod); }
        base = Zeta_GetMulMod(base, base, mod);
    }

    return ret;
}

int Zeta_GetLogFloor(size_t val, size_t base) {
    ZETA_DebugAssert(0 < val);
    ZETA_DebugAssert(1 < base);

    int ret = 0;

    for (; base <= val; val /= base) { ++ret; }

    return ret;
}

int Zeta_GetLogCeil(size_t val, size_t base) {
    ZETA_DebugAssert(0 < val);
    ZETA_DebugAssert(1 < base);

    int ret = 0;

    for (; 1 < val; val /= base) { ++ret; }

    return ret;
}

size_t Zeta_FindNextConMod(size_t beg, size_t target, size_t mod) {
    ZETA_DebugAssert(0 < mod);
    return beg + (target + mod - beg % mod) % mod;
}

int Zeta_GetLSB(size_t x) {
    if (x == 0) { return -1; }

    const int table[] = { 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };

    int ret = 0;

    size_t tmp;

    for (; (tmp = x % 65536) == 0; x /= 65536) { ret += 16; }

    if ((tmp = x % 256) == 0) {
        ret += 8;
        x /= 256;
    } else {
        x = tmp;
    }

    if ((tmp = x % 16) == 0) {
        ret += 4;
        x /= 16;
    } else {
        x = tmp;
    }

    return ret + table[x % 16];
}

int Zeta_GetMSB(size_t x) {
    if (x == 0) { return -1; }

    const int table[] = { 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3 };

    int ret = 0;

    if (65536 <= x) {
        ret += 16;
        x /= 65536;
    }

    if (256 <= x) {
        ret += 8;
        x /= 256;
    }

    if (16 <= x) {
        ret += 4;
        x /= 16;
    }

    return ret + table[x];
}

void* Zeta_GetMostLink(void* context, void* (*GetLink)(void* context, void* n),
                       void* n) {
    ZETA_DebugAssert(GetLink != NULL);

    void* m = n;

    while (m != NULL) {
        n = m;
        m = GetLink(context, n);
    }

    return n;
}
