#include "utils.h"

#include "Debugger.h"

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

unsigned long long Zeta_ReadLittleEndianStd(byte_t const* data,
                                            unsigned int length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(u128_t));

    unsigned long long ret = 0;

    for (unsigned int i = length; 0 < i--;) { ret = ret * 256 + data[i]; }

    return ret;
}

u128_t Zeta_ReadLittleEndian(byte_t const* data, unsigned int length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(u128_t));

    u128_t ret = 0;

    for (unsigned int i = length; 0 < i--;) { ret = ret * 256 + data[i]; }

    return ret;
}

unsigned long long Zeta_WriteLittleEndianStd(byte_t* dst,
                                             unsigned long long val,
                                             unsigned int length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned int i = 0; i < length; ++i) {
        dst[i] = val % 256;
        val /= 256;
    }

    return val;
}

u128_t Zeta_WriteLittleEndian(byte_t* dst, u128_t val, unsigned int length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned int i = 0; i < length; ++i) {
        dst[i] = val % 256;
        val /= 256;
    }

    return val;
}

unsigned long long Zeta_ReadBigEndianStd(byte_t const* data,
                                         unsigned int length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(unsigned long long));

    unsigned long long ret = 0;

    for (unsigned int i = 0; i < length; ++i) { ret = ret * 256 + data[i]; }

    return ret;
}

u128_t Zeta_ReadBigEndian(byte_t const* data, unsigned int length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(u128_t));

    u128_t ret = 0;

    for (unsigned int i = 0; i < length; ++i) { ret = ret * 256 + data[i]; }

    return ret;
}

unsigned long long Zeta_WriteBigEndianStd(byte_t* dst, unsigned long long val,
                                          unsigned int length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned int i = length; 0 < i--;) {
        dst[i] = val % 256;
        val /= 256;
    }

    return val;
}

u128_t Zeta_WriteBigEndian(byte_t* dst, u128_t val, unsigned int length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned int i = length; 0 < i--;) {
        dst[i] = val % 256;
        val /= 256;
    }

    return val;
}

unsigned long long Zeta_SimpleRandom(unsigned long long x) {
    unsigned long long mask = 0xFFFF;  // 2^16 - 1
    unsigned long long a = 25214903917;
    unsigned long long c = 11;

    return ((x * a + c) >> 16) & mask;
}

unsigned long long Zeta_SimpleRandomRotate(unsigned long long* x) {
    unsigned long long m = 0x80000000;
    unsigned long long a = 0x41C64E6D;
    unsigned long long c = 3039;

    return (*x = (*x * a + c) % m) / 0x10000;
}

unsigned long long Zeta_SimpleHash(unsigned long long x) {
    x = (x ^ (x >> 30)) * (unsigned long long)0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * (unsigned long long)0x94d049bb133111eb;
    x = x ^ (x >> 31);
    return x;
}

unsigned long long Zeta_GetGCD(unsigned long long x, unsigned long long y) {
    if (x == 0 && y == 0) { return 1; }

    for (;;) {
        if (x == 0) { return y; }
        y %= x;
        if (y == 0) { return x; }
        x %= y;
    }
}

unsigned long long Zeta_GetLCM(unsigned long long x, unsigned long long y) {
    return x / Zeta_GetGCD(x, y) * y;
}

unsigned long long Zeta_GetPower(unsigned long long base,
                                 unsigned long long exp) {
    unsigned long long ret = 1;

    for (; 0 < exp; exp /= 2) {
        if (exp % 2 == 1) { ret *= base; }
        base *= base;
    }

    return ret;
}

unsigned long long Zeta_GetMulMod(unsigned long long x, unsigned long long y,
                                  unsigned long long mod) {
    ZETA_DebugAssert(0 < mod);
    ZETA_DebugAssert(mod <= ZETA_GetMaxMod(unsigned long long));

    x %= mod;
    y %= mod;

    if (x == 0 || y <= ZETA_RangeMaxOf(unsigned long long) / x) {
        return (x * y) % mod;
    }

    if (x < y) { ZETA_Swap(x, y); }

    unsigned long long ret = 0;

    for (; 0 < y; y /= 2) {
        if (y % 2 == 1) { ret = (ret + x) % mod; }
        x = (x + x) % mod;
    }

    return ret;
}

unsigned long long Zeta_GetPowerMod(unsigned long long base,
                                    unsigned long long exp,
                                    unsigned long long mod) {
    ZETA_DebugAssert(0 < mod);
    ZETA_DebugAssert(mod <= ZETA_GetMaxMod(unsigned long long));

    base %= mod;

    if (base == 0) { return 0; }

    unsigned long long ret = 1;

    for (; 0 < exp; exp /= 2) {
        if (exp % 2 == 1) { ret = Zeta_GetMulMod(ret, base, mod); }
        base = Zeta_GetMulMod(base, base, mod);
    }

    return ret;
}

unsigned int Zeta_GetFloorLog(unsigned long long val, unsigned long long base) {
    ZETA_DebugAssert(0 < val);
    ZETA_DebugAssert(1 < base);

    unsigned int ret = 0;

    if (ZETA_WidthOf(unsigned long long) / base < base) { goto L1; }

    unsigned long long base_2 = base * base;
    if (ZETA_WidthOf(unsigned long long) / base_2 < base_2) { goto L2; }

    unsigned long long base_4 = base_2 * base_2;

    for (; base_4 <= val; val /= base_4) { ret += 4; }

L2:;
    for (; base_2 <= val; val /= base_2) { ret += 2; }

L1:;
    for (; base <= val; val /= base) { ret += 1; }

    return ret;
}

unsigned int Zeta_GetCeilLog(unsigned long long val, unsigned long long base) {
    ZETA_DebugAssert(0 < val);
    ZETA_DebugAssert(1 < base);

    unsigned int ret = 0;

    if (ZETA_WidthOf(unsigned long long) / base < base) { goto L1; }

    unsigned long long base_2 = base * base;
    if (ZETA_WidthOf(unsigned long long) / base_2 < base_2) { goto L2; }

    unsigned long long base_4 = base_2 * base_2;

    for (; base_4 <= val; val /= base_4) { ret += 4; }

L2:;
    for (; base_2 <= val; val /= base_2) { ret += 2; }

L1:;
    for (; base <= val; val /= base) { ret += 1; }

    return ret + (1 < val);
}

unsigned long long Zeta_GetFloorSqrt(unsigned long long val) {
    if (val <= 1) { return val; }

    unsigned long long x = 1;

    {
        unsigned long long a = val;

        for (; 256 <= a; a /= 256) { x *= 16; }

        if (16 <= a) {
            a /= 16;
            x *= 4;
        }

        if (1 < a) {
            a /= 4;
            x *= 2;
        }

        if (1 < a) { x *= 2; }
    }

    unsigned long long y = (val / x + x) / 2;

    while (y < x) {
        x = y;
        y = (val / x + x) / 2;
    }

    return x;
}

unsigned long long Zeta_FindNextConMod(unsigned long long beg,
                                       unsigned long long target,
                                       unsigned long long mod) {
    ZETA_DebugAssert(0 < mod);
    return beg + (target + mod - beg % mod) % mod;
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
