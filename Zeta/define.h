#pragma once

#include "limits.h"
#include "stdalign.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#if defined(__cplusplus)
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
#define ZETA_DebugAssert(cond) ZETA_Unused(cond)
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

#define ZETA_Identity(x) x

#define ZETA_ToStr_(x) #x
#define ZETA_ToStr(x) ZETA_ToStr_(x)

#define ZETA_Concat_(x, y) x##y
#define ZETA_Concat(x, y) ZETA_Concat_(x, y)

#if defined(__cplusplus)
#define ZETA_PrintVar(var)                                                     \
    std::cout << __FILE__ ":" << ZETA_ToStr(__LINE__) "\t" #var " = " << (var) \
              << '\n';
#else
#define ZETA_PrintVar(var)                                     \
    printf(__FILE__ ":" ZETA_ToStr(__LINE__) "\t" #var " = "); \
                                                               \
    _Generic((var),                                            \
        char: printf("%c\n", (var)),                           \
        unsigned char: printf("%X\n", (var)),                  \
        signed char: printf("%c\n", (var)),                    \
                                                               \
        short: printf("%i\n", (var)),                          \
        unsigned short: printf("%u\n", (var)),                 \
                                                               \
        int: printf("%i\n", (var)),                            \
        unsigned int: printf("%u\n", (var)),                   \
                                                               \
        long int: printf("%li\n", (var)),                      \
        unsigned long int: printf("%llu\n", (var)),            \
                                                               \
        long long int: printf("%lli\n", (var)),                \
        unsigned long long int: printf("%llu\n", (var)),       \
                                                               \
        float: printf("%g\n", (var)),                          \
                                                               \
        double: printf("%g\n", (var)),                         \
        long double: printf("%g\n", (var)),                    \
                                                               \
        void*: printf("%p\n", (var)),                          \
        const void*: printf("%p\n", (var)),                    \
                                                               \
        char*: printf("%s\n", (var)),                          \
        const char*: printf("%s\n", (var)));                   \
                                                               \
    fflush(stdout);                                            \
    ZETA_StaticAssert(TRUE)
#endif

/*
#define ZETA_PrintVar(formater, var)                                    \
    printf(__FILE__ ":%i\t" #var " = " formater "\n", __LINE__, (var)); \
    fflush(stdout);                                                     \
    ZETA_StaticAssert(TRUE)
*/

#define ZETA_Print(...)                                         \
    printf(__FILE__ ":" ZETA_ToStr(__LINE__) "\t" __VA_ARGS__); \
    fflush(stdout);                                             \
    ZETA_StaticAssert(TRUE)

#define ZETA_PrintPos                               \
    printf(__FILE__ ":" ZETA_ToStr(__LINE__) "\n"); \
    fflush(stdout);                                 \
    ZETA_StaticAssert(TRUE)

#define ZETA_Pause                                              \
    {                                                           \
        printf(__FILE__ ":" ZETA_ToStr(__LINE__) "\tpause..."); \
        char tmp;                                               \
        scanf("%c", &tmp);                                      \
    }                                                           \
    ZETA_StaticAssert(TRUE)

#define ZETA_Unused(x)          \
    if (FALSE) { ((void)(x)); } \
    ZETA_StaticAssert(TRUE)

#define ZETA_Assert(cond)                                            \
    if (cond) {                                                      \
    } else {                                                         \
        printf("assert at " __FILE__                                 \
               ":" ZETA_ToStr(__LINE__) "\n" ZETA_ToStr(cond) "\n"); \
        exit(0);                                                     \
    }                                                                \
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
        unsigned int: 0,                     \
                                             \
        long int: LONG_MIN,                  \
        unsigned long int: 0,                \
                                             \
        long long int: LLONG_MIN,            \
        unsigned long long int: 0,           \
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
        long int: LONG_MAX,                     \
        unsigned long int: ULONG_MAX,           \
                                                \
        long long int: LLONG_MAX,               \
        unsigned long long int: ULLONG_MAX,     \
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

#define ZETA_GetAddrFromPtr(x) ((uintptr_t)(void*)(x))

#define ZETA_GetPtrFromAddr(x) ((void*)(uintptr_t)(x))

#define ZETA_GetStructFromMember(type, mem, ptr) \
    ((type*)((unsigned char*)(ptr)-offsetof(type, mem)))

#define ZETA_Swap(x, y)      \
    {                        \
        typeof(x) tmp = (x); \
        (x) = (y);           \
        (y) = tmp;           \
    }                        \
    ZETA_StaticAssert(TRUE)

#define ZETA_GetMaxMod(type) (ZETA_GetRangeMax(type) / 2 + 1)

ZETA_StaticAssert(ZETA_GetRangeMin(byte_t) <= 0);
ZETA_StaticAssert(255 <= ZETA_GetRangeMax(byte_t));
