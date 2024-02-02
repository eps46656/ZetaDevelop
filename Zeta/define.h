#pragma once

#include <limits.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(bool_t)
#define bool_t _Bool
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

#define ZETA_PRINT_POS printf("print pos at %s:%d\n", __FILE__, __LINE__);

#define ZETA_UNUSED(x) ((void)(x))

#define ZETA_STATIC_ASSERT(cond) _Static_assert(cond, "");

#define ZETA_ASSERT(cond)                                      \
    if (cond) {                                                \
    } else {                                                   \
        printf("debug assert at %s:%d\n", __FILE__, __LINE__); \
        exit(0);                                               \
    }

#if defined(DEBUG)
#define ZETA_DEBUG 1
#define ZETA_DEBUG_ASSERT(cond) ZETA_ASSERT(cond)
#else
#define ZETA_DEBUG 0
#define ZETA_DEBUG_ASSERT(cond) \
    if (cond) {                 \
    } else {                    \
    }
#endif

#define ZETA_PRINT_VAR(formater, d)                                           \
    printf("print var at %s:%d\n    %s = " formater "\n", __FILE__, __LINE__, \
           #d, d);

#define ZETA_MINOF(type)                     \
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

#define ZETA_MAXOF(type)                        \
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

#define ZETA_SIGNED(type) (ZETA_MINOF(type) < 0)

#define ZETA_DECL_STRUCT(type) \
    typedef struct type type;  \
    struct type

#define ZETA_ADDR_OFFSET(ptr, offset) \
    ((void *)(intptr_t)((intptr_t)(void *)(ptr) + offset))

#define ZETA_FROM_MEM(type, mem, ptr) \
    ((type *)(ZETA_ADDR_OFFSET(ptr, -offsetof(type, mem))))

#define ZETA_SWAP(x, y)      \
    {                        \
        typeof(x) tmp = (x); \
        (x) = (y);           \
        (y) = tmp;           \
    }
