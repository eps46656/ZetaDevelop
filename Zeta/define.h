#pragma once

#include "limits.h"
#include "stdalign.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#if defined(__cplusplus)
#include <iomanip>
#include <iostream>
#endif

#if defined(__cplusplus)
#define bool_t bool
#else
#define bool_t _Bool
#endif

#define TRUE (0 == 0)
#define FALSE (0 != 0)

#define ZETA_StaticAssert(cond) _Static_assert(cond, "")

#if defined(DEBUG)
#define ZETA_IsDebug TRUE
#define ZETA_DebugAssert(cond) ZETA_Assert(cond)
#else
#define ZETA_IsDebug FALSE
#define ZETA_DebugAssert(cond)
#endif

typedef unsigned char byte_t;

typedef long long diff_t;

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

#define ZETA_Identity(x) x

#define ZETA_ToStr_(x) #x
#define ZETA_ToStr(x) ZETA_ToStr_(x)

#define ZETA_Concat_(x, y) x##y
#define ZETA_Concat(x, y) ZETA_Concat_(x, y)

#define ZETA_DeclareStruct(struct_name)     \
    typedef struct struct_name struct_name; \
    struct struct_name;                     \
    ZETA_StaticAssert(TRUE)

#define ZETA_PrintPos_                                         \
    printf("%-24s:%-4d\t%-24s", __FILE__, __LINE__, __func__); \
    ZETA_StaticAssert(TRUE)

#define ZETA_PrintPos \
    ZETA_PrintPos_;   \
    fflush(stdout);   \
    ZETA_StaticAssert(TRUE)

#if defined(__cplusplus)

#define ZETA_PrintVar(var) \
    ZETA_PrintPos_;        \
                           \
    std::cout << '\t' << ZETA_ToStr(var) << " = " << (var) << '\n';

#else

#define ZETA_PrintVar(var)               \
    ZETA_PrintPos_;                      \
                                         \
    printf("\t%s = ", ZETA_ToStr(var));  \
                                         \
    printf(_Generic((var),               \
           char: "%c\n",                 \
           unsigned char: "%X\n",        \
           signed char: "%c\n",          \
                                         \
           short: "%i\n",                \
           unsigned short: "%u\n",       \
                                         \
           int: "%i\n",                  \
           unsigned: "%u\n",             \
                                         \
           long: "%li\n",                \
           unsigned long: "%llu\n",      \
                                         \
           long long: "%lli\n",          \
           unsigned long long: "%llu\n", \
                                         \
           float: "%g\n",                \
                                         \
           double: "%g\n",               \
           long double: "%g\n",          \
                                         \
           void*: "%p\n",                \
           const void*: "%p\n",          \
                                         \
           char*: "%s\n",                \
           const char*: "%s\n"),         \
           (var));                       \
                                         \
    fflush(stdout);                      \
    ZETA_StaticAssert(TRUE)

#endif

#define ZETA_Pause            \
    {                         \
        ZETA_PrintPos_;       \
                              \
        printf("\tpause..."); \
                              \
        fflush(stdout);       \
                              \
        char tmp;             \
        scanf("%c", &tmp);    \
    }                         \
    ZETA_StaticAssert(TRUE)

#define ZETA_Unused(x)          \
    if (FALSE) { ((void)(x)); } \
    ZETA_StaticAssert(TRUE)

#define ZETA_Assert(cond)     \
    if (cond) {               \
    } else {                  \
        ZETA_PrintPos_;       \
                              \
        printf("\tassert\n"); \
                              \
        fflush(stdout);       \
        exit(0);              \
    }                         \
    ZETA_StaticAssert(TRUE)

#if defined(__cplusplus)
#define ZETA_ExternC_Beg \
    extern "C" {         \
    ZETA_StaticAssert(TRUE)
#define ZETA_ExternC_End \
    }                    \
    ZETA_StaticAssert(TRUE)
#else
#define ZETA_ExternC_Beg ZETA_StaticAssert(TRUE)
#define ZETA_ExternC_End ZETA_StaticAssert(TRUE)
#endif

#define ZETA_GetRangeMin(type)               \
    _Generic((type)0,                        \
        char: CHAR_MIN,                      \
        unsigned char: 0,                    \
        signed char: SCHAR_MIN,              \
                                             \
        short: SHRT_MIN,                     \
        unsigned short: 0,                   \
                                             \
        int: INT_MIN,                        \
        unsigned: 0,                         \
                                             \
        long: LONG_MIN,                      \
        unsigned long: 0,                    \
                                             \
        long long: LLONG_MIN,                \
        unsigned long long: 0,               \
                                             \
        u8_t: ((u8_t)0),                     \
        s8_t: ((s8_t)(-((s8_t)1 << 7))),     \
                                             \
        u16_t: ((u16_t)0),                   \
        s16_t: ((s16_t)(-((s16_t)1 << 15))), \
                                             \
        u32_t: ((u32_t)0),                   \
        s32_t: ((s32_t)(-((s32_t)1 << 31))), \
                                             \
        u64_t: ((u64_t)0),                   \
        s64_t: ((s64_t)(-((s64_t)1 << 63))), \
                                             \
        u128_t: ((u128_t)0),                 \
        s128_t: ((s128_t)(-((s128_t)1 << 127))))

#define ZETA_GetRangeMax(type)                  \
    _Generic((type)0,                           \
        char: CHAR_MAX,                         \
        unsigned char: UCHAR_MAX,               \
        signed char: SCHAR_MAX,                 \
                                                \
        short: SHRT_MAX,                        \
        unsigned short: USHRT_MAX,              \
                                                \
        int: INT_MAX,                           \
        unsigned int: UINT_MAX,                 \
                                                \
        long: LONG_MAX,                         \
        unsigned long: ULONG_MAX,               \
                                                \
        long long: LLONG_MAX,                   \
        unsigned long long: ULLONG_MAX,         \
                                                \
        u8_t: ((u8_t)(~(u8_t)0)),               \
        s8_t: ((s8_t)(((s8_t)1 << 7) - 1)),     \
                                                \
        u16_t: ((u16_t)(~(u16_t)0)),            \
        s16_t: ((s16_t)(((s16_t)1 << 15) - 1)), \
                                                \
        u32_t: ((u32_t)(~(u32_t)0)),            \
        s32_t: ((s32_t)(((s32_t)1 << 31) - 1)), \
                                                \
        u64_t: ((u64_t)(~(u64_t)0)),            \
        s64_t: ((s64_t)(((s64_t)1 << 63) - 1)), \
                                                \
        u128_t: ((u128_t)(~(u128_t)0)),         \
        s128_t: ((s128_t)(((s128_t)1 << 127) - 1)))

#define ZETA_IsSigned(type) (ZETA_GetRangeMin(type) != 0)

#define ZETA_GetWidth(type)                           \
    _Generic((type)0,                                 \
        char: CHAR_BIT,                               \
        unsigned char: CHAR_BIT,                      \
        signed char: CHAR_BIT,                        \
                                                      \
        unsigned: (__builtin_clz(1) + 1),             \
        unsigned long: (__builtin_clzl(1) + 1),       \
        unsigned long long: (__builtin_clzll(1) + 1), \
                                                      \
        u8_t: (8),                                    \
        u16_t: (16),                                  \
        u32_t: (32),                                  \
        u64_t: (64),                                  \
        u128_t: (128))

#define ZETA_GetAddrFromPtr(x) ((uintptr_t)(void*)(x))

#define ZETA_GetPtrFromAddr(x) ((void*)(uintptr_t)(x))

#define ZETA_GetStructFromMember(struct_type, member_name, member_ptr) \
    ((struct_type*)((unsigned char*)(member_ptr) -                     \
                    offsetof(struct_type, member_name)))

#define ZETA_Swap(type, x, y) \
    {                         \
        type tmp = (x);       \
        (x) = (y);            \
        (y) = tmp;            \
    }                         \
    ZETA_StaticAssert(TRUE)

#define ZETA_GetMaxMod(type) (ZETA_GetRangeMax(type) / 2 + 1)

ZETA_StaticAssert(ZETA_GetRangeMin(byte_t) <= 0);
ZETA_StaticAssert(255 <= ZETA_GetRangeMax(byte_t));
