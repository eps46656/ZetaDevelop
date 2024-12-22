#include "utils.h"

#include "debugger.h"

void Zeta_HelloWorld() { printf("Hello World.\n"); }

void Zeta_MemCopy(void* dst, void const* src, size_t size) {
    if (dst == src || size == 0) { return; }

    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    ZETA_DebugAssert(!ZETA_AreOverlapped(dst, dst + size, src, src + size));

    __builtin_memcpy(dst, src, size);
}

void Zeta_MemMove(void* dst, void const* src, size_t size) {
    if (dst == src || size == 0) { return; }

    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    __builtin_memmove(dst, src, size);
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

void Zeta_MemReverse(void* data, size_t stride, size_t size) {
    ZETA_DebugAssert(data != NULL);

    if (stride == 0 || size <= 1) { return; }

    void* i = data;
    void* j = data + stride * (size - 1);

    for (; i < j; i += stride, j -= stride) { Zeta_MemSwap(i, j, stride); }
}

unsigned long long Zeta_MemHash(void const* data_, size_t size) {
    unsigned long long fnv_offset_basis = 14695981039346656037ULL;
    unsigned long long fnv_prime = 1099511628211ULL;

    unsigned char const* data = data_;
    ZETA_DebugAssert(data != NULL);

    unsigned long long ret = fnv_offset_basis;

    for (size_t i = 0; i < size; ++i) {
        ret *= fnv_prime;
        ret ^= data[i];
    }

    return ret;
}

void Zeta_ElemCopy(void* dst, void const* src, size_t width, size_t stride,
                   size_t size) {
    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    if (size == 0 || dst == src) { return; }

    size_t length = stride * (size - 1) + width;

    if (stride - width == 0) {
        Zeta_MemCopy(dst, src, length);
        return;
    }

    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    ZETA_DebugAssert(!ZETA_AreOverlapped(dst, dst + length, src, src + length));

    for (size_t i = 0; i < size; ++i, dst += stride, src += stride) {
        Zeta_MemCopy(dst, src, width);
    }
}

void Zeta_ElemMove(void* dst, void const* src, size_t width, size_t stride,
                   size_t size) {
    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    if (size == 0 || dst == src) { return; }

    size_t length = stride * (size - 1) + width;

    if (stride - width == 0) {
        Zeta_MemMove(dst, src, length);
        return;
    }

    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    size_t del;

    if (ZETA_AreOverlapped(dst, dst + length, src, src + length) && src < dst) {
        dst += stride * size;
        src += stride * size;
        del = -stride;
    } else {
        dst -= stride;
        src -= stride;
        del = stride;
    }

    for (size_t i = 0; i < size; ++i) {
        Zeta_MemMove(dst += del, src += del, width);
    }
}

unsigned long long Zeta_ReadLittleEndianULL(byte_t const* src,
                                            unsigned length) {
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_ULLONG_WIDTH);

    unsigned long long ret = 0;

    for (unsigned i = length; 0 < i--;) { ret = ret * 256 + src[i]; }

    return ret;
}

u128_t Zeta_ReadLittleEndian(byte_t const* src, unsigned length) {
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(u128_t));

    u128_t ret = 0;

    for (unsigned i = length; 0 < i--;) { ret = ret * 256 + src[i]; }

    return ret;
}

unsigned long long Zeta_WriteLittleEndianULL(byte_t* dst,
                                             unsigned long long val,
                                             unsigned length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned i = 0; i < length; ++i) {
        dst[i] = val % 256;
        val /= 256;
    }

    return val;
}

u128_t Zeta_WriteLittleEndian(byte_t* dst, u128_t val, unsigned length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned i = 0; i < length; ++i) {
        dst[i] = val % 256;
        val /= 256;
    }

    return val;
}

unsigned long long Zeta_ReadBigEndianULL(byte_t const* src, unsigned length) {
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_ULLONG_WIDTH);

    unsigned long long ret = 0;

    for (unsigned i = 0; i < length; ++i) { ret = ret * 256 + src[i]; }

    return ret;
}

u128_t Zeta_ReadBigEndian(byte_t const* src, unsigned length) {
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(u128_t));

    u128_t ret = 0;

    for (unsigned i = 0; i < length; ++i) { ret = ret * 256 + src[i]; }

    return ret;
}

unsigned long long Zeta_WriteBigEndianULL(byte_t* dst, unsigned long long val,
                                          unsigned length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned i = length; 0 < i--;) {
        dst[i] = val % 256;
        val /= 256;
    }

    return val;
}

u128_t Zeta_WriteBigEndian(byte_t* dst, u128_t val, unsigned length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned i = length; 0 < i--;) {
        dst[i] = val % 256;
        val /= 256;
    }

    return val;
}

byte_t const* Zeta_ReadStr(byte_t const* src, size_t src_size) {
    for (; 0 < src_size && *src != '\0'; --src_size, ++src) {}
    return src;
}

unsigned long long Zeta_ULLHash(unsigned long long x, unsigned long long salt) {
    x ^= salt;

#if ZETA_ULLONG_WIDTH == 32
    x = (x ^ (x >> 16)) * 0x45d9f3b;
    x = (x ^ (x >> 16)) * 0x45d9f3b;
    x = x ^ (x >> 16);
#elif ZETA_ULLONG_WIDTH == 64
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    x = x ^ (x >> 31);
#else
#error "Unsupported machine architecture."
#endif

    x ^= salt;

    return x;
}

unsigned long long Zeta_LLHash(long long x, unsigned long long salt) {
    return Zeta_ULLHash(x, salt);
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

unsigned long long Zeta_GCD(unsigned long long x, unsigned long long y) {
    if (x == 0) { return y == 0 ? 1 : y; }
    if (y == 0) { return x; }
    while ((x %= y) != 0 && (y %= x) != 0) {}
    return x + y;
}

unsigned long long Zeta_LCM(unsigned long long x, unsigned long long y) {
    return x / Zeta_GCD(x, y) * y;
}

unsigned long long Zeta_Power(unsigned long long base, unsigned long long exp) {
    if (base == 0) { return 0; }

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
    ZETA_DebugAssert(mod <= ZETA_MaxModOf(unsigned long long));

    x %= mod;
    y %= mod;

    if (x == 0 || y <= ZETA_ULLONG_MAX / x) { return (x * y) % mod; }

    if (x < y) { ZETA_Swap(x, y); }

    unsigned long long ret = 0;

    for (; 0 < y; y /= 2) {
        if (y % 2 == 1) { ret = (ret + x) % mod; }
        x = (x + x) % mod;
    }

    return ret;
}

unsigned long long Zeta_PowerMod(unsigned long long base,
                                 unsigned long long exp,
                                 unsigned long long mod) {
    ZETA_DebugAssert(0 < mod);
    ZETA_DebugAssert(mod <= ZETA_MaxModOf(unsigned long long));

    ZETA_DebugAssert(base != 0 || exp != 0);

    base %= mod;

    if (base == 0) { return 0; }

    unsigned long long ret = 1;

    for (; 0 < exp; exp /= 2) {
        if (exp % 2 == 1) { ret = Zeta_GetMulMod(ret, base, mod); }
        base = Zeta_GetMulMod(base, base, mod);
    }

    return ret;
}

unsigned Zeta_FloorLog(unsigned long long val, unsigned long long base) {
    ZETA_DebugAssert(0 < val);
    ZETA_DebugAssert(1 < base);

    unsigned ret = 0;

    for (;;) {
        unsigned long long cur_ret = ZETA_FloorLog2(val) / ZETA_CeilLog2(base);

        if (cur_ret <= 1) { break; }

        ret += cur_ret;

        val /= Zeta_Power(base, cur_ret);
    }

    for (; base <= val; val /= base) { ++ret; }

    return ret;
}

unsigned Zeta_CeilLog(unsigned long long val, unsigned long long base) {
    ZETA_DebugAssert(1 < base);

    return val <= 1 ? 0 : Zeta_FloorLog((val - 1) / base, base) + 2;
}

unsigned long long Zeta_FixedPoint2Power(long long val_) {
#if ZETA_ULLONG_WIDTH == 32
    const unsigned long long c_base_order = 15;

    const unsigned long long c_base = 1ULL << c_base_order;

    const unsigned long long coeffs[] = {
        46341, 38968, 35734, 34219, 33486, 33125, 32946, 32857,
        32812, 32790, 32779, 32774, 32771, 32769, 32769,
    };
#elif ZETA_ULLONG_WIDTH == 64
    const unsigned long long c_base_order = 31;

    const unsigned long long c_base = 1ULL << c_base_order;

    const unsigned long long coeffs[] = {
        3037000500, 2553802834, 2341847524, 2242560872, 2194507417, 2170868212,
        2159144272, 2153306067, 2150392887, 2148937775, 2148210589, 2147847087,
        2147665360, 2147574502, 2147529075, 2147506361, 2147495005, 2147489326,
        2147486487, 2147485068, 2147484358, 2147484003, 2147483825, 2147483737,
        2147483692, 2147483670, 2147483659, 2147483654, 2147483651, 2147483649,
        2147483649,
    };
#else
#error "Unsupported machine architecture."
#endif

    ZETA_StaticAssert(ZETA_FixedPoint_BaseOrder <= c_base_order);

    const unsigned long long base = ZETA_FixedPoint_Base;

    bool_t is_neg = val_ < 0;

    unsigned long long val = is_neg ? -val_ : val_;

    unsigned long long k = val / base;

    val = val % base * (c_base / base);

    unsigned long long y = c_base;

    for (int i = 0; i < ZETA_FixedPoint_BaseOrder; ++i) {
        if (c_base <= (val *= 2)) {
            val -= c_base;
            y = ZETA_RoundIntDiv(y * coeffs[i], c_base);
        }
    }

    if (is_neg) {
        long long l = c_base_order + ZETA_FixedPoint_BaseOrder - k;
        return l < 0 ? 0 : (1ULL << l) / y;
    }

    long long l = k - c_base_order + ZETA_FixedPoint_BaseOrder;

    return l < 0 ? y / (1ULL << -l) : y * (1ULL << l);
}

long long Zeta_FixedPointLog2(unsigned long long val) {
    ZETA_DebugAssert(0 < val);

    const unsigned long long c_base_order = ZETA_ULLONG_WIDTH / 2 - 1;

    const unsigned long long c_base = 1ULL << c_base_order;

    if (val == 0) { return ZETA_SLLONG_MIN; }

    if (val == 1ULL << ZETA_FixedPoint_BaseOrder) { return 0; }

    unsigned long long k = ZETA_FloorLog2(val);

    unsigned long long mantissa =
        k <= c_base_order ? val * (1ULL << (c_base_order - k))
                          : ZETA_RoundIntDiv(val, 1ULL << (k - c_base_order));

    long long y = 0;

    for (int i = 0; i < ZETA_FixedPoint_BaseOrder; ++i) {
        mantissa *= mantissa;
        y *= 2;

        if (mantissa < c_base * c_base * 2) {
            mantissa = ZETA_RoundIntDiv(mantissa, c_base);
        } else {
            ++y;
            mantissa = ZETA_RoundIntDiv(mantissa, c_base * 2);
        }
    }

    return ((long long)k - (long long)ZETA_FixedPoint_BaseOrder) *
               (long long)ZETA_FixedPoint_Base +
           y;
}

unsigned long long Zeta_FloorSqrt(unsigned long long val) {
    if (val <= 1) { return val; }

    unsigned long long x = 1ULL << ((ZETA_CeilLog2(val) + 1) / 2);

    unsigned long long y = (val / x + x) / 2;

    while (y < x) {
        x = y;
        y = (val / x + x) / 2;
    }

    return x;
}

unsigned long long Zeta_CeilSqrt(unsigned long long val) {
    if (val <= 1) { return val; }

    unsigned long long k = Zeta_FloorSqrt(val);

    return k + (k * k <= val);
}

unsigned long long Zeta_FixedPointSqrt(unsigned long long val) {
    unsigned long long base = ZETA_FixedPoint_Base;

    const unsigned long long c_base_order = ZETA_ULLONG_WIDTH / 2 - 1;

    const unsigned long long c_base = 1ULL << c_base_order;

    unsigned long long k = Zeta_FloorSqrt(val / base);

    unsigned long long res = (val - k * k * base) * (c_base / base);

    unsigned long long lb = 0;
    unsigned long long rb = c_base - 1;

    while (lb < rb) {
        unsigned long long mb = (lb + rb + 1) / 2;

        if (2 * k * mb + ZETA_RoundIntDiv(mb * mb, c_base) < res) {
            lb = mb;
        } else {
            rb = mb - 1;
        }
    }

    return k * base + ZETA_RoundIntDiv(lb * base, c_base);
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

int Zeta_Choose2(bool_t a_cond, bool_t b_cond, unsigned long long* rand_seed) {
    switch ((int)b_cond * 2 + (int)a_cond) {
        case 0b00: return -1;
        case 0b01: return 0;
        case 0b10: return 1;
    }

    return Zeta_SimpleRandomRotate(rand_seed) % 2;
}

int Zeta_Choose3(bool_t a_cond, bool_t b_cond, bool_t c_cond,
                 unsigned long long* rand_seed) {
    int vec = (int)c_cond * 4 + (int)b_cond * 2 + (int)(a_cond);

    switch (vec) {
        case 0b000: return -1;
        case 0b001: return 0;
        case 0b010: return 1;
        case 0b100: return 2;
    }

    int k = Zeta_SimpleRandomRotate(rand_seed);

    switch (vec) {
        case 0b011: return k % 2;
        case 0b101: return (k % 2) * 2;
        case 0b110: return k % 2 + 1;
    }

    return k % 3;
}

int Zeta_ChooseN(bool_t* conds, size_t n, unsigned long long* rand_seed) {
    if (n == 2) { return Zeta_Choose2(conds[0], conds[1], rand_seed); }

    if (n == 3) {
        return Zeta_Choose3(conds[0], conds[1], conds[2], rand_seed);
    }

    size_t ln = n / 2;
    size_t rn = n - ln;

    if (Zeta_SimpleRandomRotate(rand_seed) % n < ln) {
        int ret = Zeta_ChooseN(conds, ln, rand_seed);
        if (0 <= ret) { return ret; }

        ret = Zeta_ChooseN(conds + ln, rn, rand_seed);
        return 0 <= ret ? ln + ret : -1;
    } else {
        int ret = Zeta_ChooseN(conds + ln, rn, rand_seed);
        if (0 <= ret) { return ln + ret; }

        ret = Zeta_ChooseN(conds, ln, rand_seed);
        return 0 <= ret ? ret : -1;
    }
}
