#include "utils.h"

void Zeta_MemCopy(diff_t size, char* dst, char* src) {
    ZETA_DEBUG_ASSERT(0 <= size);

    if (dst == src) { return; }

    if (src < dst && dst < src + size) {
        for (diff_t i = size - 1; 0 <= i; --i) { dst[i] = src[i]; }
    } else {
        for (diff_t i = 0; i < size; ++i) { dst[i] = src[i]; }
    }
}

void Zeta_MemSwap(diff_t size, char* x, char* y) {
    ZETA_DEBUG_ASSERT(0 <= size);

    if (x == y) { return; }

    for (diff_t i = 0; i < size; ++i) {
        char tmp = x[i];
        x[i] = y[i];
        y[i] = tmp;
    }
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

size_t Zeta_SimpleRandom(size_t x) {
    ZETA_DEBUG_ASSERT(32 <= CHAR_BIT * sizeof(size_t));

    size_t mask = 0xffff;  // 2^16 - 1
    size_t a = 25214903917;
    size_t c = 11;

    return ((x * a + c) >> 16) & mask;
}

size_t Zeta_SimpleHash(size_t x) {
    for (size_t i = ZETA_MAXOF(size_t); 0 < i; i >>= 16) {
        x = ((x >> 16) ^ x) * 0x45d9f3b;
    }

    x = (x >> 16) ^ x;
    return x;
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
