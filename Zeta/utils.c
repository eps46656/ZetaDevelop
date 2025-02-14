#include "utils.h"

#include "debugger.h"

void Zeta_HelloWorld() { printf("Hello World.\n"); }

int Zeta_MemCompare(void const* a_, void const* b_, size_t size) {
    unsigned char const* a = a_;
    unsigned char const* b = b_;

    if (a == b || size == 0) { return 0; }

    ZETA_DebugAssert(a != NULL);
    ZETA_DebugAssert(b != NULL);

    for (size_t i = 0; i < size; ++i) {
        int cmp = ZETA_ThreeWayCompare(a[i], b[i]);
        if (cmp != 0) { return cmp; }
    }

    return 0;
}

void Zeta_MemCopy(void* dst, void const* src, size_t size) {
    if (dst == src || size == 0) { return; }

    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    __builtin_memcpy(dst, src, size);
}

void Zeta_MemMove(void* dst, void const* src, size_t size) {
    if (dst == src || size == 0) { return; }

    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    __builtin_memmove(dst, src, size);
}

void Zeta_MemSwap(void* x_, void* y_, size_t size) {
    char* x = x_;
    char* y = y_;

    if (x == y) { return; }

    ZETA_DebugAssert(x != NULL);
    ZETA_DebugAssert(y != NULL);

    for (size_t i = 0; i < size; ++i) { ZETA_Swap(x[i], y[i]); }
}

void* Zeta_MemRotate(void* data, size_t l_size, size_t r_size) {
    if (l_size == 0 && r_size == 0) { return data; }

    ZETA_DebugAssert(data != NULL);

    void* ret = data + r_size;

    void* end = data + l_size + r_size;

    while (0 < l_size && 0 < r_size) {
        char* iter = data;
        char* jter = data + l_size;

        for (; jter != end; ++iter, ++jter) { ZETA_Swap(*iter, *jter); }

        r_size %= l_size;
        l_size -= r_size;
    }

    return ret;
}

void Zeta_MemReverse(void* data, size_t stride, size_t size) {
    if (stride == 0 || size <= 1) {
        if (size == 1) { ZETA_DebugAssert(data != NULL); }
        return;
    }

    void* i = data;
    void* j = data + stride * (size - 1);

    for (; i < j; i += stride, j -= stride) { Zeta_MemSwap(i, j, stride); }
}

unsigned long long Zeta_MemHash(void const* data_, size_t size,
                                unsigned long long salt) {
    unsigned long long fnv_offset_basis = 14695981039346656037ULL;
    unsigned long long fnv_prime = 1099511628211ULL;

    unsigned char const* data = data_;
    ZETA_DebugAssert(data != NULL);

    unsigned long long ret = fnv_offset_basis;

    for (size_t i = 0; i < size; ++i) {
        ret *= fnv_prime;
        ret = ret ^ data[i] + salt;
    }

    return ret;
}

int Zeta_ElemCompare(void const* a, void const* b, size_t width,
                     size_t a_stride, size_t b_stride, size_t size) {
    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= a_stride);
    ZETA_DebugAssert(width <= b_stride);

    if (size == 0) { return 0; }

    if (width == a_stride && width == b_stride) {
        return Zeta_MemCompare(a, b, a_stride * size);
    }

    for (size_t i = 0; i < size; ++i) {
        int cmp = Zeta_MemCompare(a + a_stride * i, b + b_stride * i, width);
        if (cmp != 0) { return cmp; }
    }

    return 0;
}

void Zeta_ElemCopy(void* dst, void const* src, size_t width, size_t dst_stride,
                   size_t src_stride, size_t size) {
    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= dst_stride);
    ZETA_DebugAssert(width <= src_stride);

    if (size == 0) { return; }

    if (width == dst_stride && width == src_stride) {
        Zeta_MemCopy(dst, src, width * size);
        return;
    }

    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    for (; 0 < size--; dst += dst_stride, src += src_stride) {
        Zeta_MemCopy(dst, src, width);
    }
}

void Zeta_ElemMove(void* dst, void const* src, size_t width, size_t dst_stride,
                   size_t src_stride, size_t size) {
    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(0 < dst_stride);
    ZETA_DebugAssert(0 < src_stride);

    if (size == 0) { return; }

    width = ZETA_GetMinOf(width, ZETA_GetMinOf(dst_stride, src_stride));

    if (width == dst_stride && width == src_stride) {
        Zeta_MemMove(dst, src, width * size);
        return;
    }

    ZETA_DebugAssert(dst != NULL);
    ZETA_DebugAssert(src != NULL);

    void* dst_end = dst + dst_stride * (size - 1) + width;
    void const* src_end = src + src_stride * (size - 1) + width;

    if (dst_end <= src || src_end <= dst) {
        Zeta_ElemCopy(dst, src, width, dst_stride, src_stride, size);
        return;
    }

    if (dst == src && dst_stride == src_stride) { return; }

    if (dst <= src && dst_end <= src_end) {
        for (; 0 < size--; dst += dst_stride, src += src_stride) {
            Zeta_MemCopy(dst, src, width);
        }

        return;
    }

    if (src <= dst && src_end <= dst_end) {
        dst += dst_stride * size;
        src += src_stride * size;

        while (0 < size--) {
            Zeta_MemCopy(dst -= dst_stride, src -= src_stride, width);
        }

        return;
    }

    size_t buffer_capacity = ZETA_FloorLog2(size) + 4;

    size_t* begs = __builtin_alloca_with_align(
        sizeof(size_t) * buffer_capacity, __CHAR_BIT__ * alignof(max_align_t));

    size_t* cnts = __builtin_alloca_with_align(
        sizeof(size_t) * buffer_capacity, __CHAR_BIT__ * alignof(max_align_t));

    size_t buffer_i = 0;

    begs[buffer_i] = 0;
    cnts[buffer_i] = size;
    ++buffer_i;

    while (0 < buffer_i--) {
        size_t cur_beg = begs[buffer_i];
        size_t cur_cnt = cnts[buffer_i];

        if (cur_cnt == 1) {
            Zeta_MemMove(dst + dst_stride * cur_beg, src + src_stride * cur_beg,
                         width);

            continue;
        }

        size_t cur_l_cnt = cur_cnt / 2;
        size_t cur_r_cnt = cur_cnt - cur_l_cnt;

        void* dst_mid = dst + dst_stride * cur_l_cnt;
        void const* src_mid = src + src_stride * cur_l_cnt;

        if (dst_mid <= src_mid) {
            begs[buffer_i] = cur_beg + cur_l_cnt;
            cnts[buffer_i] = cur_r_cnt;
            ++buffer_i;

            begs[buffer_i] = cur_beg;
            cnts[buffer_i] = cur_l_cnt;
            ++buffer_i;
        } else {
            begs[buffer_i] = cur_beg;
            cnts[buffer_i] = cur_l_cnt;
            ++buffer_i;

            begs[buffer_i] = cur_beg + cur_l_cnt;
            cnts[buffer_i] = cur_r_cnt;
            ++buffer_i;
        }
    }
}

void* Zeta_ElemRotate(void* data, size_t width, size_t stride, size_t l_size,
                      size_t r_size) {
    if (width == stride) {
        return Zeta_MemRotate(data, stride * l_size, stride * r_size);
    }

    if (l_size == 0 && r_size == 0) { return data; }

    ZETA_DebugAssert(data != NULL);

    void* ret = data + r_size;

    void* end = data + l_size + r_size;

    while (0 < l_size && 0 < r_size) {
        void* iter = data;
        void* jter = data + l_size;

        for (; jter != end; iter += stride, jter += stride) {
            Zeta_MemSwap(iter, jter, width);
        }

        r_size %= l_size;
        l_size -= r_size;
    }

    return ret;
}

u128_t Zeta_ReadLittleEndian(byte_t const* src, unsigned length) {
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(u128_t));

    u128_t ret = 0;

    for (unsigned i = length; 0 < i--;) { ret = (ret << 8) + src[i]; }

    return ret;
}

u128_t Zeta_WriteLittleEndian(byte_t* dst, u128_t val, unsigned length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned i = 0; i < length; ++i) {
        dst[i] = val % (1 << 8);
        val >>= 8;
    }

    return val;
}

u128_t Zeta_ReadBigEndian(byte_t const* src, unsigned length) {
    ZETA_DebugAssert(src != NULL);
    ZETA_DebugAssert(8 * length <= ZETA_WidthOf(u128_t));

    u128_t ret = 0;

    for (unsigned i = 0; i < length; ++i) { ret = (ret << 8) + src[i]; }

    return ret;
}

u128_t Zeta_WriteBigEndian(byte_t* dst, u128_t val, unsigned length) {
    ZETA_DebugAssert(dst != NULL);

    for (unsigned i = length; 0 < i--;) {
        dst[i] = val % (1 << 8);
        val >>= 8;
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
    x = (x ^ (x >> 16)) * 0x45d9f3bULL;
    x = (x ^ (x >> 16)) * 0x45d9f3bULL;
    x = x ^ (x >> 16);
#elif ZETA_ULLONG_WIDTH == 64
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    x = x ^ (x >> 31);
#else
#error "Unsupported architecture."
#endif

    x ^= salt;

    return x;
}

unsigned long long Zeta_LLHash(long long x, unsigned long long salt) {
    return Zeta_ULLHash(x, salt);
}

#if ZETA_ULLONG_WIDTH == 32
#define LCG_MUL (0x1010101ULL)
#define LCG_INC (0x24924907ULL)
#elif ZETA_ULLONG_WIDTH == 64
#define LCG_MUL (0x1010101ULL)
#define LCG_INC (0x2492492492492479ULL)
#else
#error "Unsupported architecture."
#endif

static unsigned long long random_seed_ = ZETA_PtrToAddr(&random_seed_) - 1;

unsigned long long Zeta_GetRandom() {
    random_seed_ =
        ((random_seed_ + __builtin_readcyclecounter()) * LCG_MUL + LCG_INC);

    return Zeta_ULLHash(random_seed_, __builtin_readcyclecounter());
}

unsigned long long Zeta_SimpleRandomRotate(unsigned long long* x) {
    return Zeta_ULLHash(*x = (*x * LCG_MUL + LCG_INC), 0);
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

unsigned long long Zeta_Power(unsigned long long base, unsigned exp) {
    if (base == 0) { return 0; }

    if (__builtin_popcountll(base) == 1) {
        return 1ULL << (__builtin_ctzll(base) * exp);
    }

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
    ZETA_DebugAssert(mod <= ZETA_ULLONG_MAX);

    x %= mod;
    y %= mod;

    unsigned long long ret = 0;

    if (!__builtin_umulll_overflow(x, y, &ret)) { return ret % mod; }

    if (x < y) { ZETA_Swap(x, y); }

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
    ZETA_DebugAssert(mod <= ZETA_ULLONG_MAX);

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

    if (base == 2) { return ZETA_FloorLog2(val); }

    unsigned ceil_log2_base = ZETA_CeilLog2(base);

    unsigned ret = 0;

    for (;;) {
        unsigned cur_ret = ZETA_FloorLog2(val) / ceil_log2_base;

        ret += cur_ret;

        switch (cur_ret) {
            case 1: val /= base;
            case 0: goto RES;
        }

        val /= Zeta_Power(base, cur_ret);
    }

RES:

    for (; base <= val; val /= base) { ++ret; }

    return ret;
}

unsigned Zeta_CeilLog(unsigned long long val, unsigned long long base) {
    ZETA_DebugAssert(1 < base);

    if (base == 2) { return ZETA_CeilLog2(val); }

    return val <= 1 ? 0 : Zeta_FloorLog(val - 1, base) + 1;
}

unsigned long long Zeta_FixedPoint2Power(long long val_) {
#if ZETA_ULLONG_WIDTH == 32
    unsigned long long const c_base_order = 15;

    unsigned long long const c_base = 1ULL << c_base_order;

    unsigned long long const coeffs[] = {
        46341, 38968, 35734, 34219, 33486, 33125, 32946, 32857,
        32812, 32790, 32779, 32774, 32771, 32769, 32769,
    };
#elif ZETA_ULLONG_WIDTH == 64
    unsigned long long const c_base_order = 31;

    unsigned long long const c_base = 1ULL << c_base_order;

    unsigned long long const coeffs[] = {
        3037000500, 2553802834, 2341847524, 2242560872, 2194507417, 2170868212,
        2159144272, 2153306067, 2150392887, 2148937775, 2148210589, 2147847087,
        2147665360, 2147574502, 2147529075, 2147506361, 2147495005, 2147489326,
        2147486487, 2147485068, 2147484358, 2147484003, 2147483825, 2147483737,
        2147483692, 2147483670, 2147483659, 2147483654, 2147483651, 2147483649,
        2147483649,
    };
#else
#error "Unsupported architecture."
#endif

    ZETA_StaticAssert(ZETA_FixedPoint_BaseOrder <= c_base_order);

    unsigned long long const base = ZETA_FixedPoint_Base;

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

    return l < 0 ? (y >> -l) : (y << l);
}

long long Zeta_FixedPointLog2(unsigned long long val) {
    ZETA_DebugAssert(0 < val);

    unsigned long long const c_base_order = ZETA_ULLONG_WIDTH / 2 - 1;

    unsigned long long const c_base = 1ULL << c_base_order;

    if (val == 0) { return ZETA_SLLONG_MIN; }

    if (val == 1ULL << ZETA_FixedPoint_BaseOrder) { return 0; }

    unsigned long long k = ZETA_FloorLog2(val);

    unsigned long long mantissa =
        k <= c_base_order ? (val << (c_base_order - k))
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

    unsigned long long y = (x + val / x) / 2;

    while (y < x) {
        x = y;
        y = (x + val / x) / 2;
    }

    return x;
}

unsigned long long Zeta_CeilSqrt(unsigned long long val) {
    if (val <= 1) { return val; }

    unsigned long long k = Zeta_FloorSqrt(val);

    return k + (k * k < val);
}

unsigned long long Zeta_FixedPointSqrt(unsigned long long val) {
    if (ZETA_FixedPoint_BaseOrder <= __builtin_clzll(val)) {
        return Zeta_FloorSqrt(val << ZETA_FixedPoint_BaseOrder);
    }

    udllong_t ex_val = (udllong_t)(val) << ZETA_FixedPoint_BaseOrder;

    udllong_t x = (udllong_t)(1)
                  << ((ZETA_CeilLog2(val) + ZETA_FixedPoint_BaseOrder + 1) / 2);

    udllong_t y = (x + ex_val / x) / 2;

    while (y < x) {
        x = y;
        y = (x + ex_val / x) / 2;
    }

    return x;
}

unsigned long long Zeta_FindNextConMod(unsigned long long beg,
                                       unsigned long long target,
                                       unsigned long long mod) {
    ZETA_DebugAssert(0 < mod);
    return beg + (target + mod - beg % mod) % mod;
}

void* Zeta_GetMostLink(void* n, void* (*GetLink)(void* n)) {
    ZETA_DebugAssert(GetLink != NULL);

    void* m = n;

    while (m != NULL) {
        n = m;
        m = GetLink(n);
    }

    return n;
}

/*
static signed char nth_set_bit_table[256][8] = {
    { -1, -1, -1, -1, -1, -1, -1, -1 }, { 0, -1, -1, -1, -1, -1, -1, -1 },
    { 1, -1, -1, -1, -1, -1, -1, -1 },  { 0, 1, -1, -1, -1, -1, -1, -1 },
    { 2, -1, -1, -1, -1, -1, -1, -1 },  { 0, 2, -1, -1, -1, -1, -1, -1 },
    { 1, 2, -1, -1, -1, -1, -1, -1 },   { 0, 1, 2, -1, -1, -1, -1, -1 },
    { 3, -1, -1, -1, -1, -1, -1, -1 },  { 0, 3, -1, -1, -1, -1, -1, -1 },
    { 1, 3, -1, -1, -1, -1, -1, -1 },   { 0, 1, 3, -1, -1, -1, -1, -1 },
    { 2, 3, -1, -1, -1, -1, -1, -1 },   { 0, 2, 3, -1, -1, -1, -1, -1 },
    { 1, 2, 3, -1, -1, -1, -1, -1 },    { 0, 1, 2, 3, -1, -1, -1, -1 },
    { 4, -1, -1, -1, -1, -1, -1, -1 },  { 0, 4, -1, -1, -1, -1, -1, -1 },
    { 1, 4, -1, -1, -1, -1, -1, -1 },   { 0, 1, 4, -1, -1, -1, -1, -1 },
    { 2, 4, -1, -1, -1, -1, -1, -1 },   { 0, 2, 4, -1, -1, -1, -1, -1 },
    { 1, 2, 4, -1, -1, -1, -1, -1 },    { 0, 1, 2, 4, -1, -1, -1, -1 },
    { 3, 4, -1, -1, -1, -1, -1, -1 },   { 0, 3, 4, -1, -1, -1, -1, -1 },
    { 1, 3, 4, -1, -1, -1, -1, -1 },    { 0, 1, 3, 4, -1, -1, -1, -1 },
    { 2, 3, 4, -1, -1, -1, -1, -1 },    { 0, 2, 3, 4, -1, -1, -1, -1 },
    { 1, 2, 3, 4, -1, -1, -1, -1 },     { 0, 1, 2, 3, 4, -1, -1, -1 },
    { 5, -1, -1, -1, -1, -1, -1, -1 },  { 0, 5, -1, -1, -1, -1, -1, -1 },
    { 1, 5, -1, -1, -1, -1, -1, -1 },   { 0, 1, 5, -1, -1, -1, -1, -1 },
    { 2, 5, -1, -1, -1, -1, -1, -1 },   { 0, 2, 5, -1, -1, -1, -1, -1 },
    { 1, 2, 5, -1, -1, -1, -1, -1 },    { 0, 1, 2, 5, -1, -1, -1, -1 },
    { 3, 5, -1, -1, -1, -1, -1, -1 },   { 0, 3, 5, -1, -1, -1, -1, -1 },
    { 1, 3, 5, -1, -1, -1, -1, -1 },    { 0, 1, 3, 5, -1, -1, -1, -1 },
    { 2, 3, 5, -1, -1, -1, -1, -1 },    { 0, 2, 3, 5, -1, -1, -1, -1 },
    { 1, 2, 3, 5, -1, -1, -1, -1 },     { 0, 1, 2, 3, 5, -1, -1, -1 },
    { 4, 5, -1, -1, -1, -1, -1, -1 },   { 0, 4, 5, -1, -1, -1, -1, -1 },
    { 1, 4, 5, -1, -1, -1, -1, -1 },    { 0, 1, 4, 5, -1, -1, -1, -1 },
    { 2, 4, 5, -1, -1, -1, -1, -1 },    { 0, 2, 4, 5, -1, -1, -1, -1 },
    { 1, 2, 4, 5, -1, -1, -1, -1 },     { 0, 1, 2, 4, 5, -1, -1, -1 },
    { 3, 4, 5, -1, -1, -1, -1, -1 },    { 0, 3, 4, 5, -1, -1, -1, -1 },
    { 1, 3, 4, 5, -1, -1, -1, -1 },     { 0, 1, 3, 4, 5, -1, -1, -1 },
    { 2, 3, 4, 5, -1, -1, -1, -1 },     { 0, 2, 3, 4, 5, -1, -1, -1 },
    { 1, 2, 3, 4, 5, -1, -1, -1 },      { 0, 1, 2, 3, 4, 5, -1, -1 },
    { 6, -1, -1, -1, -1, -1, -1, -1 },  { 0, 6, -1, -1, -1, -1, -1, -1 },
    { 1, 6, -1, -1, -1, -1, -1, -1 },   { 0, 1, 6, -1, -1, -1, -1, -1 },
    { 2, 6, -1, -1, -1, -1, -1, -1 },   { 0, 2, 6, -1, -1, -1, -1, -1 },
    { 1, 2, 6, -1, -1, -1, -1, -1 },    { 0, 1, 2, 6, -1, -1, -1, -1 },
    { 3, 6, -1, -1, -1, -1, -1, -1 },   { 0, 3, 6, -1, -1, -1, -1, -1 },
    { 1, 3, 6, -1, -1, -1, -1, -1 },    { 0, 1, 3, 6, -1, -1, -1, -1 },
    { 2, 3, 6, -1, -1, -1, -1, -1 },    { 0, 2, 3, 6, -1, -1, -1, -1 },
    { 1, 2, 3, 6, -1, -1, -1, -1 },     { 0, 1, 2, 3, 6, -1, -1, -1 },
    { 4, 6, -1, -1, -1, -1, -1, -1 },   { 0, 4, 6, -1, -1, -1, -1, -1 },
    { 1, 4, 6, -1, -1, -1, -1, -1 },    { 0, 1, 4, 6, -1, -1, -1, -1 },
    { 2, 4, 6, -1, -1, -1, -1, -1 },    { 0, 2, 4, 6, -1, -1, -1, -1 },
    { 1, 2, 4, 6, -1, -1, -1, -1 },     { 0, 1, 2, 4, 6, -1, -1, -1 },
    { 3, 4, 6, -1, -1, -1, -1, -1 },    { 0, 3, 4, 6, -1, -1, -1, -1 },
    { 1, 3, 4, 6, -1, -1, -1, -1 },     { 0, 1, 3, 4, 6, -1, -1, -1 },
    { 2, 3, 4, 6, -1, -1, -1, -1 },     { 0, 2, 3, 4, 6, -1, -1, -1 },
    { 1, 2, 3, 4, 6, -1, -1, -1 },      { 0, 1, 2, 3, 4, 6, -1, -1 },
    { 5, 6, -1, -1, -1, -1, -1, -1 },   { 0, 5, 6, -1, -1, -1, -1, -1 },
    { 1, 5, 6, -1, -1, -1, -1, -1 },    { 0, 1, 5, 6, -1, -1, -1, -1 },
    { 2, 5, 6, -1, -1, -1, -1, -1 },    { 0, 2, 5, 6, -1, -1, -1, -1 },
    { 1, 2, 5, 6, -1, -1, -1, -1 },     { 0, 1, 2, 5, 6, -1, -1, -1 },
    { 3, 5, 6, -1, -1, -1, -1, -1 },    { 0, 3, 5, 6, -1, -1, -1, -1 },
    { 1, 3, 5, 6, -1, -1, -1, -1 },     { 0, 1, 3, 5, 6, -1, -1, -1 },
    { 2, 3, 5, 6, -1, -1, -1, -1 },     { 0, 2, 3, 5, 6, -1, -1, -1 },
    { 1, 2, 3, 5, 6, -1, -1, -1 },      { 0, 1, 2, 3, 5, 6, -1, -1 },
    { 4, 5, 6, -1, -1, -1, -1, -1 },    { 0, 4, 5, 6, -1, -1, -1, -1 },
    { 1, 4, 5, 6, -1, -1, -1, -1 },     { 0, 1, 4, 5, 6, -1, -1, -1 },
    { 2, 4, 5, 6, -1, -1, -1, -1 },     { 0, 2, 4, 5, 6, -1, -1, -1 },
    { 1, 2, 4, 5, 6, -1, -1, -1 },      { 0, 1, 2, 4, 5, 6, -1, -1 },
    { 3, 4, 5, 6, -1, -1, -1, -1 },     { 0, 3, 4, 5, 6, -1, -1, -1 },
    { 1, 3, 4, 5, 6, -1, -1, -1 },      { 0, 1, 3, 4, 5, 6, -1, -1 },
    { 2, 3, 4, 5, 6, -1, -1, -1 },      { 0, 2, 3, 4, 5, 6, -1, -1 },
    { 1, 2, 3, 4, 5, 6, -1, -1 },       { 0, 1, 2, 3, 4, 5, 6, -1 },
    { 7, -1, -1, -1, -1, -1, -1, -1 },  { 0, 7, -1, -1, -1, -1, -1, -1 },
    { 1, 7, -1, -1, -1, -1, -1, -1 },   { 0, 1, 7, -1, -1, -1, -1, -1 },
    { 2, 7, -1, -1, -1, -1, -1, -1 },   { 0, 2, 7, -1, -1, -1, -1, -1 },
    { 1, 2, 7, -1, -1, -1, -1, -1 },    { 0, 1, 2, 7, -1, -1, -1, -1 },
    { 3, 7, -1, -1, -1, -1, -1, -1 },   { 0, 3, 7, -1, -1, -1, -1, -1 },
    { 1, 3, 7, -1, -1, -1, -1, -1 },    { 0, 1, 3, 7, -1, -1, -1, -1 },
    { 2, 3, 7, -1, -1, -1, -1, -1 },    { 0, 2, 3, 7, -1, -1, -1, -1 },
    { 1, 2, 3, 7, -1, -1, -1, -1 },     { 0, 1, 2, 3, 7, -1, -1, -1 },
    { 4, 7, -1, -1, -1, -1, -1, -1 },   { 0, 4, 7, -1, -1, -1, -1, -1 },
    { 1, 4, 7, -1, -1, -1, -1, -1 },    { 0, 1, 4, 7, -1, -1, -1, -1 },
    { 2, 4, 7, -1, -1, -1, -1, -1 },    { 0, 2, 4, 7, -1, -1, -1, -1 },
    { 1, 2, 4, 7, -1, -1, -1, -1 },     { 0, 1, 2, 4, 7, -1, -1, -1 },
    { 3, 4, 7, -1, -1, -1, -1, -1 },    { 0, 3, 4, 7, -1, -1, -1, -1 },
    { 1, 3, 4, 7, -1, -1, -1, -1 },     { 0, 1, 3, 4, 7, -1, -1, -1 },
    { 2, 3, 4, 7, -1, -1, -1, -1 },     { 0, 2, 3, 4, 7, -1, -1, -1 },
    { 1, 2, 3, 4, 7, -1, -1, -1 },      { 0, 1, 2, 3, 4, 7, -1, -1 },
    { 5, 7, -1, -1, -1, -1, -1, -1 },   { 0, 5, 7, -1, -1, -1, -1, -1 },
    { 1, 5, 7, -1, -1, -1, -1, -1 },    { 0, 1, 5, 7, -1, -1, -1, -1 },
    { 2, 5, 7, -1, -1, -1, -1, -1 },    { 0, 2, 5, 7, -1, -1, -1, -1 },
    { 1, 2, 5, 7, -1, -1, -1, -1 },     { 0, 1, 2, 5, 7, -1, -1, -1 },
    { 3, 5, 7, -1, -1, -1, -1, -1 },    { 0, 3, 5, 7, -1, -1, -1, -1 },
    { 1, 3, 5, 7, -1, -1, -1, -1 },     { 0, 1, 3, 5, 7, -1, -1, -1 },
    { 2, 3, 5, 7, -1, -1, -1, -1 },     { 0, 2, 3, 5, 7, -1, -1, -1 },
    { 1, 2, 3, 5, 7, -1, -1, -1 },      { 0, 1, 2, 3, 5, 7, -1, -1 },
    { 4, 5, 7, -1, -1, -1, -1, -1 },    { 0, 4, 5, 7, -1, -1, -1, -1 },
    { 1, 4, 5, 7, -1, -1, -1, -1 },     { 0, 1, 4, 5, 7, -1, -1, -1 },
    { 2, 4, 5, 7, -1, -1, -1, -1 },     { 0, 2, 4, 5, 7, -1, -1, -1 },
    { 1, 2, 4, 5, 7, -1, -1, -1 },      { 0, 1, 2, 4, 5, 7, -1, -1 },
    { 3, 4, 5, 7, -1, -1, -1, -1 },     { 0, 3, 4, 5, 7, -1, -1, -1 },
    { 1, 3, 4, 5, 7, -1, -1, -1 },      { 0, 1, 3, 4, 5, 7, -1, -1 },
    { 2, 3, 4, 5, 7, -1, -1, -1 },      { 0, 2, 3, 4, 5, 7, -1, -1 },
    { 1, 2, 3, 4, 5, 7, -1, -1 },       { 0, 1, 2, 3, 4, 5, 7, -1 },
    { 6, 7, -1, -1, -1, -1, -1, -1 },   { 0, 6, 7, -1, -1, -1, -1, -1 },
    { 1, 6, 7, -1, -1, -1, -1, -1 },    { 0, 1, 6, 7, -1, -1, -1, -1 },
    { 2, 6, 7, -1, -1, -1, -1, -1 },    { 0, 2, 6, 7, -1, -1, -1, -1 },
    { 1, 2, 6, 7, -1, -1, -1, -1 },     { 0, 1, 2, 6, 7, -1, -1, -1 },
    { 3, 6, 7, -1, -1, -1, -1, -1 },    { 0, 3, 6, 7, -1, -1, -1, -1 },
    { 1, 3, 6, 7, -1, -1, -1, -1 },     { 0, 1, 3, 6, 7, -1, -1, -1 },
    { 2, 3, 6, 7, -1, -1, -1, -1 },     { 0, 2, 3, 6, 7, -1, -1, -1 },
    { 1, 2, 3, 6, 7, -1, -1, -1 },      { 0, 1, 2, 3, 6, 7, -1, -1 },
    { 4, 6, 7, -1, -1, -1, -1, -1 },    { 0, 4, 6, 7, -1, -1, -1, -1 },
    { 1, 4, 6, 7, -1, -1, -1, -1 },     { 0, 1, 4, 6, 7, -1, -1, -1 },
    { 2, 4, 6, 7, -1, -1, -1, -1 },     { 0, 2, 4, 6, 7, -1, -1, -1 },
    { 1, 2, 4, 6, 7, -1, -1, -1 },      { 0, 1, 2, 4, 6, 7, -1, -1 },
    { 3, 4, 6, 7, -1, -1, -1, -1 },     { 0, 3, 4, 6, 7, -1, -1, -1 },
    { 1, 3, 4, 6, 7, -1, -1, -1 },      { 0, 1, 3, 4, 6, 7, -1, -1 },
    { 2, 3, 4, 6, 7, -1, -1, -1 },      { 0, 2, 3, 4, 6, 7, -1, -1 },
    { 1, 2, 3, 4, 6, 7, -1, -1 },       { 0, 1, 2, 3, 4, 6, 7, -1 },
    { 5, 6, 7, -1, -1, -1, -1, -1 },    { 0, 5, 6, 7, -1, -1, -1, -1 },
    { 1, 5, 6, 7, -1, -1, -1, -1 },     { 0, 1, 5, 6, 7, -1, -1, -1 },
    { 2, 5, 6, 7, -1, -1, -1, -1 },     { 0, 2, 5, 6, 7, -1, -1, -1 },
    { 1, 2, 5, 6, 7, -1, -1, -1 },      { 0, 1, 2, 5, 6, 7, -1, -1 },
    { 3, 5, 6, 7, -1, -1, -1, -1 },     { 0, 3, 5, 6, 7, -1, -1, -1 },
    { 1, 3, 5, 6, 7, -1, -1, -1 },      { 0, 1, 3, 5, 6, 7, -1, -1 },
    { 2, 3, 5, 6, 7, -1, -1, -1 },      { 0, 2, 3, 5, 6, 7, -1, -1 },
    { 1, 2, 3, 5, 6, 7, -1, -1 },       { 0, 1, 2, 3, 5, 6, 7, -1 },
    { 4, 5, 6, 7, -1, -1, -1, -1 },     { 0, 4, 5, 6, 7, -1, -1, -1 },
    { 1, 4, 5, 6, 7, -1, -1, -1 },      { 0, 1, 4, 5, 6, 7, -1, -1 },
    { 2, 4, 5, 6, 7, -1, -1, -1 },      { 0, 2, 4, 5, 6, 7, -1, -1 },
    { 1, 2, 4, 5, 6, 7, -1, -1 },       { 0, 1, 2, 4, 5, 6, 7, -1 },
    { 3, 4, 5, 6, 7, -1, -1, -1 },      { 0, 3, 4, 5, 6, 7, -1, -1 },
    { 1, 3, 4, 5, 6, 7, -1, -1 },       { 0, 1, 3, 4, 5, 6, 7, -1 },
    { 2, 3, 4, 5, 6, 7, -1, -1 },       { 0, 2, 3, 4, 5, 6, 7, -1 },
    { 1, 2, 3, 4, 5, 6, 7, -1 },        { 0, 1, 2, 3, 4, 5, 6, 7 },
};

int GetKthSetBit(unsigned long long n, int k) {
    ZETA_DebugAssert(0 <= k);
    ZETA_DebugAssert(k < ZETA_ULLONG_WIDTH);

    if (__builtin_popcountll(n) <= k) { return -1; }

    int offset = 0;


}
*/

int Zeta_Choose2(bool_t a_cond, bool_t b_cond,
                 unsigned long long* random_seed) {
    switch ((int)b_cond * 2 + (int)a_cond) {
        case 0b00: return -1;
        case 0b01: return 0;
        case 0b10: return 1;
    }

    return Zeta_SimpleRandomRotate(random_seed) % 2;
}

int Zeta_Choose3(bool_t a_cond, bool_t b_cond, bool_t c_cond,
                 unsigned long long* random_seed) {
    int vec = (int)c_cond * 4 + (int)b_cond * 2 + (int)(a_cond);

    switch (vec) {
        case 0b000: return -1;
        case 0b001: return 0;
        case 0b010: return 1;
        case 0b100: return 2;
    }

    unsigned long long k = Zeta_SimpleRandomRotate(random_seed);

    switch (vec) {
        case 0b011: return k % 2;
        case 0b101: return (k % 2) * 2;
        case 0b110: return k % 2 + 1;
    }

    return k % 3;
}

int Zeta_ChooseN(bool_t* conds, size_t n, unsigned long long* random_seed) {
    if (n == 2) { return Zeta_Choose2(conds[0], conds[1], random_seed); }

    if (n == 3) {
        return Zeta_Choose3(conds[0], conds[1], conds[2], random_seed);
    }

    size_t ln = n / 2;
    size_t rn = n - ln;

    if (Zeta_SimpleRandomRotate(random_seed) % n < ln) {
        int ret = Zeta_ChooseN(conds, ln, random_seed);
        if (0 <= ret) { return ret; }

        ret = Zeta_ChooseN(conds + ln, rn, random_seed);
        return 0 <= ret ? ln + ret : -1;
    } else {
        int ret = Zeta_ChooseN(conds + ln, rn, random_seed);
        if (0 <= ret) { return ln + ret; }

        ret = Zeta_ChooseN(conds, ln, random_seed);
        return 0 <= ret ? ret : -1;
    }
}
