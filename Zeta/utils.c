#include "utils.h"

void Zeta_MemCopy(size_t size, void* dst_, void const* src_) {
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

void Zeta_MemSwap(size_t size, void* x_, void* y_) {
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

size_t Zeta_ReadLittleEndian(byte_t const* data, unsigned int length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(length <= sizeof(size_t));

    size_t ret = 0;

    for (unsigned int i = length; 0 < i--;) {
        ret = ret * (size_t)256 + (size_t)data[i];
    }

    return ret;
}

void Zeta_WriteLittleEndian(byte_t* dst, unsigned int length, size_t val) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned int i = 0; i < length; ++i) {
        dst[i] = val % 256;
        val /= 256;
    }
}

size_t Zeta_ReadBigEndian(byte_t const* data, unsigned int length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(length <= sizeof(size_t));

    size_t ret = 0;

    for (unsigned int i = 0; i < length; ++i) {
        ret = ret * (size_t)256 + (size_t)data[i];
    }

    return ret;
}

void Zeta_WriteBigEndian(byte_t* dst, unsigned int length, size_t val) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned int i = length; 0 < i--;) {
        dst[i] = val % 256;
        val /= 256;
    }
}

size_t Zeta_SimpleRandom(size_t x) {
    ZETA_DebugAssert(32 <= CHAR_BIT * sizeof(size_t));

    size_t mask = 0xFFFF;  // 2^16 - 1
    size_t a = 25214903917;
    size_t c = 11;

    return ((x * a + c) >> 16) & mask;
}

/*
size_t Zeta_SimpleHash(size_t x) {
    for (size_t i = ZETA_maxof(size_t); 0 < i; i >>= 16) {
        x = ((x >> 16) ^ x) * 0x45D9F3B;
    }

    x = (x >> 16) ^ x;
    return x;
} */

size_t Zeta_SimpleHash(size_t x_) {
    u64_t x = x_;
    x = (x ^ (x >> 30)) * (u64_t)0xbf58476d1ce4e5b9;
    x = (x ^ (x >> 27)) * (u64_t)0x94d049bb133111eb;
    x = x ^ (x >> 31);
    return x;
}

size_t Zeta_GetGCD(size_t x, size_t y) {
    for (;;) {
        if (x == 0) { return x + y; }
        y %= x;
        if (y == 0) { return x + y; }
        x %= y;
    }
}

size_t Zeta_GetLCM(size_t x, size_t y) {
    return x == 0 || y == 0 ? x + y : x / Zeta_GetGCD(x, y) * y;
}

size_t Zeta_GetPower(size_t base, size_t exp) {
    size_t ret = 1;

    for (; 0 < exp; exp /= 2) {
        if (exp % 2 == 1) { ret *= base; }
        base *= base;
    }

    return ret;
}

/*

(1) a * b <= m

(2) m = b*k + c     0 <= c < b

(1) <-> (a * b <= b*k + c) ... (1.1)

(3) (a <= m / b) <-> (a <= k)

(3) -> (1.1) ?




(1) a * b < m       0 <= a, b       0 < m

if b == 0:
    0 < m       ok

if b != 0

(2) m = b*k + c     0 <= c < b

(1) <-> (a * b < b*k + c) ... (1.1)

(3) (a < m / b) <-> (a < k) ... (3.1)

(3.1) -> (a*b < b*k) since b != 0 ... (3.2)

(3.2) -> (a*b < b*k + c) since 0 <= c ... (1.1)

(3) <-> (3.1) -> (1.1) <-> (1)

*/

size_t Zeta_GetMulMod(size_t x, size_t y, size_t mod) {
    ZETA_DebugAssert(0 < mod);

    x %= mod;
    y %= mod;

    if (x == 0 || y <= ZETA_maxof(size_t) / x) { return (x * y) % mod; }

    ZETA_DebugAssert(mod <= ZETA_max_mod_under_size_t);

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

    if (mod == 1) { return 0; }

    base %= mod;
    size_t ret = 1;

    for (; 0 < exp; exp /= 2) {
        if (exp % 2 == 1) { ret = Zeta_GetMulMod(ret, base, mod); }
        base = Zeta_GetMulMod(base, base, mod);
    }

    return ret;
}

size_t Zeta_FindNextConMod(size_t beg, size_t target, size_t mod) {
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
