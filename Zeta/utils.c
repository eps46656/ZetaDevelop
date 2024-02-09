#include "utils.h"

void Zeta_MemCopy(size_t size, byte_t* dst, byte_t* src) {
    ZETA_DebugAssert(0 <= size);

    if (dst == src) { return; }

    if (src < dst && dst < src + size) {
        for (size_t i = size; 0 < i--;) { dst[i] = src[i]; }
    } else {
        for (size_t i = 0; i < size; ++i) { dst[i] = src[i]; }
    }
}

void Zeta_MemSwap(size_t size, byte_t* x, byte_t* y) {
    ZETA_DebugAssert(0 <= size);

    if (x == y) { return; }

    for (size_t i = 0; i < size; ++i) { ZETA_Swap(x[i], y[i]); }
}

byte_t* Zeta_MemRotate(byte_t* beg, byte_t* mid, byte_t* end) {
    ZETA_DebugAssert(beg != NULL);
    ZETA_DebugAssert(mid != NULL);
    ZETA_DebugAssert(end != NULL);

    ZETA_DebugAssert(beg <= mid);
    ZETA_DebugAssert(mid <= end);

    if (beg == mid) { return end; }
    if (mid == end) { return beg; }

    ptrdiff_t a = mid - beg;
    ptrdiff_t b = end - mid;

    byte_t* ret = beg + b;

    while (0 < a && 0 < b) {
        ptrdiff_t r = b % a;

        a -= r;
        b = r;

        for (; mid != end; ++beg, ++mid) { ZETA_Swap(*beg, *mid); }

        mid -= r;
    }

    return ret;
}

size_t Zeta_ReadLittleEndian(const byte_t* data, int length) {
    ZETA_DebugAssert(0 <= length);
    ZETA_DebugAssert((size_t)length <= sizeof(size_t));

    size_t ret = 0;

    for (int i = length - 1; 0 <= i; --i) { ret = ret * 256 + (size_t)data[i]; }

    return ret;
}

void Zeta_WriteLittleEndian(byte_t* dst, size_t x, int length) {
    ZETA_DebugAssert(0 <= length);

    for (int i = 0; i < length; ++i) {
        dst[i] = x % 256;
        x /= 256;
    }
}

size_t Zeta_ReadBigEndian(const byte_t* data, int length) {
    ZETA_DebugAssert(0 <= length);
    ZETA_DebugAssert((size_t)length <= sizeof(size_t));

    size_t ret = 0;

    for (int i = 0; i < length; ++i) { ret = ret * 256 + (size_t)data[i]; }

    return ret;
}

void Zeta_WriteBigEndian(byte_t* dst, size_t x, int length) {
    ZETA_DebugAssert(0 <= length);

    for (int i = length - 1; 0 <= i; --i) {
        dst[i] = x % 256;
        x /= 256;
    }
}

size_t Zeta_SimpleRandom(size_t x) {
    ZETA_DebugAssert(32 <= CHAR_BIT * sizeof(size_t));

    size_t mask = 0xFFFF;  // 2^16 - 1
    size_t a = 25214903917;
    size_t c = 11;

    return ((x * a + c) >> 16) & mask;
}

size_t Zeta_SimpleHash(size_t x) {
    for (size_t i = ZETA_maxof(size_t); 0 < i; i >>= 16) {
        x = ((x >> 16) ^ x) * 0x45D9F3B;
    }

    x = (x >> 16) ^ x;
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
    ZETA_DebugAssert(0 <= x);
    ZETA_DebugAssert(0 <= y);
    ZETA_DebugAssert(0 < mod);

    x %= mod;
    y %= mod;

    if (x == 0 || y <= ZETA_maxof(size_t) / x) { return (x * y) % mod; }

    ZETA_DebugAssert(mod <= ZETA_max_mod_under_size_t);

    if (x < y) {
        size_t tmp = x;
        x = y;
        y = tmp;
    }

    size_t ret = 0;

    for (; 0 < y; y /= 2) {
        if (y % 2 == 1) { ret = (ret + x) % mod; }
        x = (x + x) % mod;
    }

    return ret;
}

size_t Zeta_GetPowerMod(size_t base, size_t exp, size_t mod) {
    ZETA_DebugAssert(0 <= exp);
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
