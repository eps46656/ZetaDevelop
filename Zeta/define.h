#pragma once

#include <limits.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(bool_t)
#if defined(__cplusplus)
#define bool_t bool
#else
#define bool_t _Bool
#endif
#endif

#define TRUE (0 == 0)
#define FALSE (0 != 0)

#define ZETA_StaticAssert(cond) _Static_assert(cond, "")

#if defined(DEBUG)
#define ZETA_debug 1
#define ZETA_DebugAssert(cond) ZETA_Assert(cond)
#else
#define ZETA_debug 0
#define ZETA_DebugAssert(cond) \
    if (cond) {                \
    } else {                   \
    }                          \
    ZETA_StaticAssert(TRUE)
#endif

typedef unsigned char byte_t;

typedef long long int diff_t;

typedef signed _BitInt(8) s8_t;
typedef signed _BitInt(16) s16_t;
typedef signed _BitInt(32) s32_t;
typedef signed _BitInt(64) s64_t;
typedef signed _BitInt(128) s128_t;

typedef unsigned _BitInt(8) u8_t;
typedef unsigned _BitInt(16) u16_t;
typedef unsigned _BitInt(32) u32_t;
typedef unsigned _BitInt(64) u64_t;
typedef unsigned _BitInt(128) u128_t;

typedef s32_t unichar_t;

#define ZETA_PrintVar(formater, d) \
    printf("%s:%d\t\t%s = " formater "\n", __FILE__, __LINE__, #d, d)

#define ZETA_PrintPos ZETA_PrintVar("%s", "print pos")

#define ZETA_Unused(x) ((void)(x))

#define ZETA_Assert(cond)                                      \
    if (cond) {                                                \
    } else {                                                   \
        printf("debug assert at %s:%d\n", __FILE__, __LINE__); \
        exit(0);                                               \
    }                                                          \
    ZETA_StaticAssert(TRUE)

#if defined(__cplusplus)
#define ZETA_extern_c_beg \
    extern "C" {          \
    ZETA_StaticAssert(TRUE)
#define ZETA_extern_c_end \
    }                     \
    ZETA_StaticAssert(TRUE)
#else
#define ZETA_extern_c_beg ZETA_StaticAssert(TRUE)
#define ZETA_extern_c_end ZETA_StaticAssert(TRUE)
#endif

#define ZETA_minof(type)                     \
    _Generic((type)0,                        \
                                             \
        char: CHAR_MIN,                      \
                                             \
        short: SHRT_MIN,                     \
                                             \
        int: INT_MIN,                        \
                                             \
        long int: LONG_MIN,                  \
                                             \
        long long int: LLONG_MIN,            \
                                             \
        signed char: SCHAR_MIN,              \
                                             \
        unsigned char: 0,                    \
                                             \
        unsigned short: 0,                   \
                                             \
        unsigned int: 0,                     \
                                             \
        unsigned long int: 0,                \
                                             \
        unsigned long long int: 0            \
                                             \
        u8_t: ((u8_t)0),                     \
                                             \
        s8_t: ((s8_t)(-((s8_t)1 << 7))),     \
                                             \
        u16_t: ((u16_t)0),                   \
                                             \
        s16_t: ((s16_t)(-((s16_t)1 << 15))), \
                                             \
        u32_t: ((u32_t)0),                   \
                                             \
        s32_t: ((s32_t)(-((s32_t)1 << 31))), \
                                             \
        u64_t: ((u64_t)0),                   \
                                             \
        s64_t: ((s64_t)(-((s64_t)1 << 63))), \
                                             \
        u128_t: ((u128_t)0),                 \
                                             \
        s128_t: ((s128_t)(-((s128_t)1 << 127))))

#define ZETA_maxof(type)                        \
    _Generic((type)0,                           \
                                                \
        char: CHAR_MAX,                         \
                                                \
        short: SHRT_MAX,                        \
                                                \
        int: INT_MAX,                           \
                                                \
        long int: LONG_MAX,                     \
                                                \
        long long int: LLONG_MAX,               \
                                                \
        signed char: SCHAR_MAX,                 \
                                                \
        unsigned char: UCHAR_MAX,               \
                                                \
        unsigned short: USHRT_MAX,              \
                                                \
        unsigned int: UINT_MAX,                 \
                                                \
        unsigned long int: ULONG_MAX,           \
                                                \
        unsigned long long int: ULLONG_MAX,     \
                                                \
        u8_t: ((u8_t)(~(u8_t)0)),               \
                                                \
        s8_t: ((s8_t)(((s8_t)1 << 7) - 1)),     \
                                                \
        u16_t: ((u16_t)(~(u16_t)0)),            \
                                                \
        s16_t: ((s16_t)(((s16_t)1 << 15) - 1)), \
                                                \
        u32_t: ((u32_t)(~(u32_t)0)),            \
                                                \
        s32_t: ((s32_t)(((s32_t)1 << 31) - 1)), \
                                                \
        u64_t: ((u64_t)(~(u64_t)0)),            \
                                                \
        s64_t: ((s64_t)(((s64_t)1 << 63) - 1)), \
                                                \
        u128_t: ((u128_t)(~(u128_t)0)),         \
                                                \
        s128_t: ((s128_t)(((s128_t)1 << 127) - 1)))

#define ZETA_IsSigned(type) (ZETA_minof(type) < 0)

#define ZETA_OffsetPtr(ptr, offset) \
    ((void*)(intptr_t)((intptr_t)(void*)(ptr) + offset))

#define ZETA_GetStructFromMem(type, mem, ptr) \
    ((type*)(ZETA_OffsetPtr(ptr, -offsetof(type, mem))))

#define ZETA_Swap(x, y)      \
    {                        \
        typeof(x) tmp = (x); \
        (x) = (y);           \
        (y) = tmp;           \
    }                        \
    ZETA_StaticAssert(TRUE)

#define ZETA_max_mod_under_size_t (ZETA_maxof(size_t) / 2 + 1)
