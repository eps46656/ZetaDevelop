#include "utils.h"

void Zeta_MemCopy(size_t size, byte_t* dst, byte_t* src) {
    ZETA_DEBUG_ASSERT(0 <= size);

    if (dst == src) { return; }

    if (src < dst && dst < src + size) {
        for (size_t i = size; 0 < i--;) { dst[i] = src[i]; }
    } else {
        for (size_t i = 0; i < size; ++i) { dst[i] = src[i]; }
    }
}

void Zeta_MemSwap(size_t size, byte_t* x, byte_t* y) {
    ZETA_DEBUG_ASSERT(0 <= size);

    if (x == y) { return; }

    for (size_t i = 0; i < size; ++i) {
        byte_t tmp = x[i];
        x[i] = y[i];
        y[i] = tmp;
    }
}

byte_t* Zeta_MemRotate(byte_t* beg, byte_t* mid, byte_t* end) {
    ZETA_DEBUG_ASSERT(beg != NULL);
    ZETA_DEBUG_ASSERT(mid != NULL);
    ZETA_DEBUG_ASSERT(end != NULL);

    ZETA_DEBUG_ASSERT(beg <= mid);
    ZETA_DEBUG_ASSERT(mid <= end);

    if (beg == mid) { return end; }
    if (mid == end) { return beg; }

    ptrdiff_t a = mid - beg;
    ptrdiff_t b = end - mid;

    byte_t* ret = beg + b;

    while (0 < a && 0 < b) {
        ptrdiff_t r = b % a;

        a -= r;
        b = r;

        for (; mid != end; ++beg, ++mid) {
            byte_t tmp = *beg;
            *beg = *mid;
            *mid = tmp;
        }

        mid -= r;
    }

    return ret;
}

void Zeta_Swap(diff_t* a, diff_t* b) {
    diff_t tmp = *a;
    *a = *b;
    *b = tmp;
}

size_t Zeta_PowerM(size_t base, size_t exp, size_t mod) {
    ZETA_DEBUG_ASSERT(0 <= exp);
    ZETA_DEBUG_ASSERT(1 <= mod);

    base %= mod;
    size_t ret = 1;

    for (; 0 < exp; exp /= 2) {
        if (exp % 2 == 1) { ret = (ret * base) % mod; }
        base = (base * base) % mod;
    }

    return ret;
}

size_t Zeta_ReadLittleEndian(const byte_t* data, int length) {
    ZETA_DEBUG_ASSERT(0 <= length);
    ZETA_DEBUG_ASSERT((size_t)length <= sizeof(size_t));

    size_t ret = 0;

    for (int i = length - 1; 0 <= i; --i) { ret = ret * 256 + (size_t)data[i]; }

    return ret;
}

void Zeta_WriteLittleEndian(byte_t* dst, size_t x, int length) {
    ZETA_DEBUG_ASSERT(0 <= length);

    for (int i = 0; i < length; ++i) {
        dst[i] = x % 256;
        x /= 256;
    }
}

size_t Zeta_ReadBigEndian(const byte_t* data, int length) {
    ZETA_DEBUG_ASSERT(0 <= length);
    ZETA_DEBUG_ASSERT((size_t)length <= sizeof(size_t));

    size_t ret = 0;

    for (int i = 0; i < length; ++i) { ret = ret * 256 + (size_t)data[i]; }

    return ret;
}

void Zeta_WriteBigEndian(byte_t* dst, size_t x, int length) {
    ZETA_DEBUG_ASSERT(0 <= length);

    for (int i = length - 1; 0 <= i; --i) {
        dst[i] = x % 256;
        x /= 256;
    }
}

size_t Zeta_SimpleRandom(size_t x) {
    ZETA_DEBUG_ASSERT(32 <= CHAR_BIT * sizeof(size_t));

    size_t mask = 0xFFFF;  // 2^16 - 1
    size_t a = 25214903917;
    size_t c = 11;

    return ((x * a + c) >> 16) & mask;
}

size_t Zeta_SimpleHash(size_t x) {
    for (size_t i = ZETA_MAXOF(size_t); 0 < i; i >>= 16) {
        x = ((x >> 16) ^ x) * 0x45D9F3B;
    }

    x = (x >> 16) ^ x;
    return x;
}

size_t GetGCD(size_t x, size_t y) {
    for (;;) {
        if (x == 0) { return x + y; }
        y %= x;
        if (y == 0) { return x + y; }
        x %= y;
    }
}

size_t GetLCM(size_t x, size_t y) {
    return x == 0 || y == 0 ? x + y : x / GetGCD(x, y) * y;
}

size_t FindNextConMod(size_t beg, size_t target, size_t mod) {
    ZETA_DEBUG_ASSERT(0 < mod);
    return beg + (target + mod - beg % mod) % mod;
}

void* Zeta_GetMostLink(void* context, void* (*GetLink)(void* context, void* n),
                       void* n) {
    ZETA_DEBUG_ASSERT(GetLink != NULL);

    void* m = n;

    while (m != NULL) {
        n = m;
        m = GetLink(context, n);
    }

    return n;
}
