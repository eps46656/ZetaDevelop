#include "utils.h"

#include "Debugger.h"

void Zeta_HelloWorld() { printf("Hello World.\n"); }

void Zeta_MemCopy(void* dst, void const* src, size_t size) {
    if (dst == src) { return; }

    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    ZETA_DebugAssert(!ZETA_AreOverlapped(dst, size, src, size));

    __builtin_memcpy(dst, src, size);
}

void Zeta_MemMove(void* dst, void const* src, size_t size) {
    if (dst == src) { return; }

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

void Zeta_MemReverse(void* data_, size_t stride, size_t size) {
    unsigned char* data = data_;
    ZETA_DebugAssert(data);

    if (stride == 0 || size <= 1) { return; }

    unsigned char* i = data;
    unsigned char* j = data + stride * (size - 1);

    for (; i < j; i += stride, j -= stride) { Zeta_MemSwap(i, j, stride); }
}

void Zeta_EleCopy(void* dst_, void const* src_, size_t stride, size_t width,
                  size_t size) {
    unsigned char* dst = dst_;
    unsigned char const* src = src_;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    if (size == 0 || dst == src) { return; }

    size_t length = stride * (size - 1) + width;

    if (stride - width <= sizeof(void*) * 2) {
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

void Zeta_EleMove(void* dst_, void const* src_, size_t stride, size_t width,
                  size_t size) {
    unsigned char* dst = dst_;
    unsigned char const* src = src_;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);

    if (size == 0 || dst == src) { return; }

    size_t length = stride * (size - 1) + width;

    if (stride - width <= sizeof(void*) * 2) {
        Zeta_MemMove(dst, src, length);
        return;
    }

    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    if (ZETA_AreOverlapped(dst, dst + length, src, src + length) && src < dst) {
        dst += stride * (size - 1);
        src += stride * (size - 1);

        for (size_t i = 0; i < size; ++i, dst -= stride, src -= stride) {
            Zeta_MemMove(dst, src, width);
        }
    } else {
        for (size_t i = 0; i < size; ++i, dst += stride, src += stride) {
            Zeta_MemMove(dst, src, width);
        }
    }
}

unsigned long long Zeta_ReadLittleEndianULL(byte_t const* data,
                                            unsigned length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(unsigned long long));

    unsigned long long ret = 0;

    for (unsigned i = length; 0 < i--;) { ret = ret * 256 + data[i]; }

    return ret;
}

u128_t Zeta_ReadLittleEndian(byte_t const* data, unsigned length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(u128_t));

    u128_t ret = 0;

    for (unsigned i = length; 0 < i--;) { ret = ret * 256 + data[i]; }

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

unsigned long long Zeta_ReadBigEndianULL(byte_t const* data, unsigned length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(unsigned long long));

    unsigned long long ret = 0;

    for (unsigned i = 0; i < length; ++i) { ret = ret * 256 + data[i]; }

    return ret;
}

u128_t Zeta_ReadBigEndian(byte_t const* data, unsigned length) {
    ZETA_DebugAssert(data != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(u128_t));

    u128_t ret = 0;

    for (unsigned i = 0; i < length; ++i) { ret = ret * 256 + data[i]; }

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

unsigned long long Zeta_GCD(unsigned long long x, unsigned long long y) {
    if (x == 0 && y == 0) { return 1; }

    for (;;) {
        if (x == 0) { return y; }
        y %= x;
        if (y == 0) { return x; }
        x %= y;
    }
}

unsigned long long Zeta_LCM(unsigned long long x, unsigned long long y) {
    return x / Zeta_GCD(x, y) * y;
}

unsigned long long Zeta_Power(unsigned long long base, unsigned long long exp) {
    ZETA_DebugAssert(base != 0 || exp != 0);

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

unsigned long long Zeta_PowerMod(unsigned long long base,
                                 unsigned long long exp,
                                 unsigned long long mod) {
    ZETA_DebugAssert(0 < mod);
    ZETA_DebugAssert(mod <= ZETA_GetMaxMod(unsigned long long));

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

/*
unsigned Zeta_FloorLog(unsigned long long val, unsigned long long base) {
    ZETA_DebugAssert(0 < val);
    ZETA_DebugAssert(1 < base);

    unsigned ret = 0;

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
*/

/*
#define base_exp_length (8)

#define IntLog_(is_ceil)                                               \
    ZETA_DebugAssert(0 < val);                                         \
    ZETA_DebugAssert(1 < base);                                        \
                                                                       \
    unsigned long long const K = 65535;                                \
    ZETA_StaticAssert(K <= ZETA_RangeMaxOf(unsigned long long) / K);   \
                                                                       \
    ZETA_StaticAssert(ZETA_WidthOf(unsigned long long) <=              \
                      ZETA_2Power(base_exp_length - 1));               \
                                                                       \
    unsigned long long base_exp[base_exp_length] = { base };           \
    unsigned base_exp_i = 0;                                           \
                                                                       \
    for (unsigned long long end = ZETA_GetMinOf(val, K);               \
         base_exp[base_exp_i] < end;) {                                \
        unsigned long long tmp = base_exp[base_exp_i];                 \
        base_exp[++base_exp_i] = tmp * tmp;                            \
    }                                                                  \
                                                                       \
    unsigned ret = 0;                                                  \
    unsigned inc = ZETA_2Power(base_exp_i);                            \
                                                                       \
    for (; base_exp[base_exp_i] <= val; val /= base_exp[base_exp_i]) { \
        ret += inc;                                                    \
    }                                                                  \
                                                                       \
    while (0 < base_exp_i) {                                           \
        --base_exp_i;                                                  \
        inc /= 2;                                                      \
                                                                       \
        if (base_exp[base_exp_i] <= val) {                             \
            ret += inc;                                                \
            val /= base_exp[base_exp_i];                               \
        }                                                              \
    }                                                                  \
                                                                       \
    if (is_ceil && 0 < val) { ++ret; }                                 \
                                                                       \
    return ret;
*/

#define IntLog_(is_ceil)                                                  \
    ZETA_DebugAssert(0 < val);                                            \
    ZETA_DebugAssert(1 < base);                                           \
                                                                          \
    unsigned long long base_exp[ZETA_WidthOf(unsigned long long) / 2] = { \
        base                                                              \
    };                                                                    \
                                                                          \
    unsigned base_exp_i = 0;                                              \
                                                                          \
    for (unsigned long long end = ZETA_GetMinOf(                          \
             val, ZETA_2Power(ZETA_WidthOf(unsigned long long) / 2));     \
         base_exp[base_exp_i] < end;) {                                   \
        unsigned long long tmp = base_exp[base_exp_i];                    \
        base_exp[++base_exp_i] = tmp * tmp;                               \
    }                                                                     \
                                                                          \
    unsigned ret = 0;                                                     \
    unsigned inc = ZETA_2Power(base_exp_i);                               \
                                                                          \
    ZETA_PrintVar(val);                                                   \
                                                                          \
    for (; base_exp[base_exp_i] <= val; val /= base_exp[base_exp_i]) {    \
        ret += inc;                                                       \
    }                                                                     \
                                                                          \
    while (0 < base_exp_i) {                                              \
        --base_exp_i;                                                     \
        inc /= 2;                                                         \
                                                                          \
        if (base_exp[base_exp_i] <= val) {                                \
            ret += inc;                                                   \
            val /= base_exp[base_exp_i];                                  \
        }                                                                 \
    }                                                                     \
                                                                          \
    ZETA_PrintVar(ret);                                                   \
                                                                          \
    if (is_ceil && 0 < val) { ++ret; }                                    \
                                                                          \
    return ret;

unsigned Zeta_FloorLog(unsigned long long val, unsigned long long base) {
    IntLog_(FALSE);
}

unsigned Zeta_CeilLog(unsigned long long val, unsigned long long base) {
    IntLog_(TRUE);
}

int Zeta_FloorLog2(unsigned long long val) {
    return val == 0 ? -1
                    : (ZETA_WidthOf(unsigned long long) - __builtin_clzll(val));
}

int Zeta_CeilLog2(unsigned long long val) {
    return val == 0 ? -1
                    : (ZETA_WidthOf(unsigned long long) - __builtin_clzll(val) -
                       (__builtin_popcountll(val) == 1));
}

unsigned long long Zeta_FixedPoint2Power(long long val_) {
    const unsigned long long c_base_order = 30;

    const unsigned long long c_base = ZETA_2Power(c_base_order);

    ZETA_StaticAssert(c_base_order * 2 <= ZETA_WidthOf(unsigned long long));

    ZETA_StaticAssert(ZETA_FixedPoint_BaseOrder <= c_base_order);

    const unsigned long long cs[] = {
        1518500250, 1276901417, 1170923762, 1121280436, 1097253708, 1085434106,
        1079572136, 1076653033, 1075196443, 1074468888, 1074105294, 1073923544,
        1073832680, 1073787251, 1073764537, 1073753181, 1073747502, 1073744663,
        1073743244, 1073742534, 1073742179, 1073742001, 1073741913, 1073741868
    };

    const unsigned long long base = ZETA_FixedPoint_Base;

    bool_t is_neg = val_ < 0;

    unsigned long long val = is_neg ? -val_ : val_;

    unsigned long long k = val / base;

    unsigned long long y = c_base;

    val = val % base * (c_base / base);

    for (int i = 0; i < ZETA_FixedPoint_BaseOrder; ++i) {
        if (c_base <= (val *= 2)) {
            val -= c_base;
            y = ZETA_RoundIntDiv(y * cs[i], c_base);
        }
    }

    if (is_neg) {
        long long l = c_base_order + ZETA_FixedPoint_BaseOrder - k;
        return l < 0 ? 0 : ZETA_2Power(l) / y;
    }

    long long l = k - c_base_order + ZETA_FixedPoint_BaseOrder;
    return l < 0 ? y / ZETA_2Power(-l) : y * ZETA_2Power(l);
}

long long Zeta_FixedPointLog2(unsigned long long val) {
    ZETA_DebugAssert(0 < val);

    const unsigned long long c_base_order = 30;

    const unsigned long long c_base = ZETA_2Power(c_base_order);

    const unsigned long long cs[] = {
        1518500250, 1276901417, 1170923762, 1121280436, 1097253708, 1085434106,
        1079572136, 1076653033, 1075196443, 1074468888, 1074105294, 1073923544,
        1073832680, 1073787251, 1073764537, 1073753181, 1073747502, 1073744663,
        1073743244, 1073742534, 1073742179, 1073742001, 1073741913, 1073741868
    };

    ZETA_StaticAssert(c_base_order * 2 <= ZETA_WidthOf(unsigned long long));

    ZETA_StaticAssert(ZETA_FixedPoint_BaseOrder <= c_base_order);

    if (val == ZETA_2Power(ZETA_FixedPoint_BaseOrder)) { return 0; }

    bool_t is_neg = val < ZETA_2Power(ZETA_FixedPoint_BaseOrder);

    if (is_neg) {
        val = ZETA_2Power(ZETA_FixedPoint_BaseOrder) *
              ZETA_2Power(ZETA_FixedPoint_BaseOrder) / val;
    }

    unsigned long long k = ZETA_FloorLog2(val);

    unsigned long long mantissa =
        k <= c_base_order
            ? val * ZETA_2Power(c_base_order - k)
            : ZETA_RoundIntDiv(val, ZETA_2Power(k - c_base_order));

    unsigned long long y = 0;

    unsigned long long l = c_base;

    for (int i = 0; i < ZETA_FixedPoint_BaseOrder; ++i) {
        unsigned long long nxt_l = ZETA_RoundIntDiv(l * cs[i], c_base);

        y *= 2;

        if (nxt_l <= mantissa) {
            ++y;
            l = nxt_l;
        }
    }

    long long ret = ((long long)k - (long long)ZETA_FixedPoint_BaseOrder) *
                        (long long)ZETA_2Power(ZETA_FixedPoint_BaseOrder) +
                    (long long)y;

    return is_neg ? -ret : ret;
}

unsigned long long Zeta_FloorSqrt(unsigned long long val) {
    if (val <= 1) { return val; }

    unsigned long long x = ZETA_2Power(ZETA_FloorLog2(val) / 2 + 1);

    unsigned long long y = (val / x + x) / 2;

    while (y < x) {
        x = y;
        y = (val / x + x) / 2;
    }

    return x;
}

unsigned long long Zeta_FixedPointSqrt(unsigned long long val) {
    unsigned long long base = ZETA_FixedPoint_Base;

    const unsigned long long c_base_order = 30;

    const unsigned long long c_base = ZETA_2Power(c_base_order);

    ZETA_StaticAssert(c_base_order * 2 <= ZETA_WidthOf(unsigned long long));

    ZETA_StaticAssert(ZETA_FixedPoint_BaseOrder <= c_base_order);

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
    return a_cond == b_cond ? Zeta_SimpleRandomRotate(rand_seed) % 2 : b_cond;
}

int Zeta_Choose3(bool_t a_cond, bool_t b_cond, bool_t c_cond,
                 unsigned long long* rand_seed) {
    int A = 0;
    int B = 1;
    int C = 2;

    switch (Zeta_SimpleRandomRotate(rand_seed) % 6) {
        case 0:  // abc
            if (a_cond) { return A; }
        case 1:  // bca
            if (b_cond) { return B; }
        case 2:  // cab
            if (c_cond) { return C; }
            if (a_cond) { return A; }
        case 3:  // bac
            if (b_cond) { return B; }
        case 4:  // acb
            if (a_cond) { return A; }
        case 5:  // cba
            if (c_cond) { return C; }
            if (b_cond) { return B; }
            if (a_cond) { return A; }
    }

    return Zeta_SimpleRandomRotate(rand_seed) % 3;
}
