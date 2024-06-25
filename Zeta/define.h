#pragma once

#include <stdalign.h>
#include <stddef.h>
#include <stdlib.h>

#define TRUE (0 == 0)
#define FALSE (0 != 0)

#if defined(__cplusplus)
#define bool_t bool
#else
#define bool_t _Bool
#endif

#define ZETA_StaticAssert(cond) _Static_assert(cond, "")

#if !defined(ZETA_IsDebug)
#if defined(DEBUG)
#define ZETA_IsDebug TRUE
#else
#define ZETA_IsDebug FALSE
#endif
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

typedef unsigned int unichar_t;

#define ZETA_ToStr_(x) #x
#define ZETA_ToStr(x) ZETA_ToStr_(x)

#define ZETA_Concat_(x, y) x##y
#define ZETA_Concat(x, y) ZETA_Concat_(x, y)

#define ZETA_UniqueName(prefix) ZETA_Concat(prefix, __COUNTER__)

#define ZETA_DeclareStruct(struct_name)     \
    typedef struct struct_name struct_name; \
    struct struct_name;                     \
    ZETA_StaticAssert(TRUE)

#define ZETA_AutoVar(var, expression) typeof((expression)) var = (expression);

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

#define ZETA_IsSigned(type)        \
    _Generic((type)0,              \
        char: ((char)(-1) < 0),    \
                                   \
        unsigned char: FALSE,      \
        unsigned short: FALSE,     \
        unsigned int: FALSE,       \
        unsigned long: FALSE,      \
        unsigned long long: FALSE, \
        u8_t: FALSE,               \
        u16_t: FALSE,              \
        u32_t: FALSE,              \
        u64_t: FALSE,              \
        u128_t: FALSE,             \
                                   \
        signed char: TRUE,         \
        signed short: TRUE,        \
        signed int: TRUE,          \
        signed long: TRUE,         \
        signed long long: TRUE,    \
        s8_t: FALSE,               \
        s16_t: FALSE,              \
        s32_t: FALSE,              \
        s64_t: FALSE,              \
        s128_t: FALSE)

#define ZETA_UIntRangeMin_(utype, stype) ((utype)(0))
#define ZETA_UIntRangeMax_(utype, stype) ((utype)(-1))

#define ZETA_SIntRangeMin_(utype, stype) \
    ((stype)(-(stype)((utype)(-1) / (utype)(2)) - (stype)(1)))
#define ZETA_SIntRangeMax_(utype, stype) ((stype)((utype)(-1) / 2))

#define ZETA_RangeMinOf(type)                                             \
    _Generic((type)0,                                                     \
        char: ZETA_IsSigned(char)                                         \
            ? ZETA_SIntRangeMin_(unsigned char, signed char)              \
            : ZETA_UIntRangeMin_(unsigned char, signed char),             \
                                                                          \
        unsigned char: ZETA_UIntRangeMin_(unsigned char, signed char),    \
                                                                          \
        unsigned short: ZETA_UIntRangeMin_(unsigned short, signed short), \
                                                                          \
        unsigned int: ZETA_UIntRangeMin_(unsigned int, signed int),       \
                                                                          \
        unsigned long: ZETA_UIntRangeMin_(unsigned long, signed long),    \
                                                                          \
        unsigned long long: ZETA_UIntRangeMin_(unsigned long long,        \
                                               signed long long),         \
                                                                          \
        u8_t: ZETA_UIntRangeMin_(u8_t, s8_t),                             \
                                                                          \
        u16_t: ZETA_UIntRangeMin_(u16_t, s16_t),                          \
                                                                          \
        u32_t: ZETA_UIntRangeMin_(u32_t, s32_t),                          \
                                                                          \
        u64_t: ZETA_UIntRangeMin_(u64_t, s64_t),                          \
                                                                          \
        u128_t: ZETA_UIntRangeMin_(u128_t, s128_t),                       \
                                                                          \
        signed char: ZETA_SIntRangeMin_(unsigned char, signed char),      \
                                                                          \
        signed short: ZETA_SIntRangeMin_(unsigned short, signed short),   \
                                                                          \
        signed int: ZETA_SIntRangeMin_(unsigned int, signed int),         \
                                                                          \
        signed long: ZETA_SIntRangeMin_(unsigned long, signed long),      \
                                                                          \
        signed long long: ZETA_SIntRangeMin_(unsigned long long,          \
                                             signed long long),           \
                                                                          \
        s8_t: ZETA_SIntRangeMin_(u8_t, s8_t),                             \
                                                                          \
        s16_t: ZETA_SIntRangeMin_(u16_t, s16_t),                          \
                                                                          \
        s32_t: ZETA_SIntRangeMin_(u32_t, s32_t),                          \
                                                                          \
        s64_t: ZETA_SIntRangeMin_(u64_t, s64_t),                          \
                                                                          \
        s128_t: ZETA_SIntRangeMin_(u128_t, s128_t))

#define ZETA_RangeMaxOf(type)                                             \
    _Generic((type)0,                                                     \
        char: ZETA_IsSigned(char)                                         \
            ? ZETA_SIntRangeMax_(unsigned char, signed char)              \
            : ZETA_UIntRangeMax_(unsigned char, signed char),             \
                                                                          \
        unsigned char: ZETA_UIntRangeMax_(unsigned char, signed char),    \
                                                                          \
        unsigned short: ZETA_UIntRangeMax_(unsigned short, signed short), \
                                                                          \
        unsigned int: ZETA_UIntRangeMax_(unsigned int, signed int),       \
                                                                          \
        unsigned long: ZETA_UIntRangeMax_(unsigned long, signed long),    \
                                                                          \
        unsigned long long: ZETA_UIntRangeMax_(unsigned long long,        \
                                               signed long long),         \
                                                                          \
        u8_t: ZETA_UIntRangeMax_(u8_t, s8_t),                             \
                                                                          \
        u16_t: ZETA_UIntRangeMax_(u16_t, s16_t),                          \
                                                                          \
        u32_t: ZETA_UIntRangeMax_(u32_t, s32_t),                          \
                                                                          \
        u64_t: ZETA_UIntRangeMax_(u64_t, s64_t),                          \
                                                                          \
        u128_t: ZETA_UIntRangeMax_(u128_t, s128_t),                       \
                                                                          \
        signed char: ZETA_SIntRangeMax_(unsigned char, signed char),      \
                                                                          \
        signed short: ZETA_SIntRangeMax_(unsigned short, signed short),   \
                                                                          \
        signed int: ZETA_SIntRangeMax_(unsigned int, signed int),         \
                                                                          \
        signed long: ZETA_SIntRangeMax_(unsigned long, signed long),      \
                                                                          \
        signed long long: ZETA_SIntRangeMax_(unsigned long long,          \
                                             signed long long),           \
                                                                          \
        s8_t: ZETA_SIntRangeMax_(u8_t, s8_t),                             \
                                                                          \
        s16_t: ZETA_SIntRangeMax_(u16_t, s16_t),                          \
                                                                          \
        s32_t: ZETA_SIntRangeMax_(u32_t, s32_t),                          \
                                                                          \
        s64_t: ZETA_SIntRangeMax_(u64_t, s64_t),                          \
                                                                          \
        s128_t: ZETA_SIntRangeMax_(u128_t, s128_t))

#define ZETA_WidthOf(type)                                                   \
    _Generic((type)0,                                                        \
        unsigned char: __builtin_popcount(ZETA_RangeMaxOf(unsigned char)),   \
                                                                             \
        unsigned short: __builtin_popcount(ZETA_RangeMaxOf(unsigned short)), \
                                                                             \
        unsigned int: __builtin_popcount(ZETA_RangeMaxOf(unsigned int)),     \
                                                                             \
        unsigned long: __builtin_popcountl(ZETA_RangeMaxOf(unsigned long)),  \
                                                                             \
        unsigned long long: __builtin_popcountll(                            \
                 ZETA_RangeMaxOf(unsigned long long)),                       \
                                                                             \
        u8_t: (8),                                                           \
                                                                             \
        u16_t: (16),                                                         \
                                                                             \
        u32_t: (32),                                                         \
                                                                             \
        u64_t: (64),                                                         \
                                                                             \
        u128_t: (128))

#define ZETA_PtrToAddr(x) ((uintptr_t)(void*)(x))
#define ZETA_AddrToPtr(x) ((void*)(uintptr_t)(x))

#define ZETA_MemberToStruct(struct_type, member_name, member_ptr) \
    ((struct_type*)((unsigned char*)(member_ptr) -                \
                    offsetof(struct_type, member_name)))

#define ZETA_GetMinOf_(x, y, x_tmp, y_tmp) \
    ({                                     \
        ZETA_AutoVar(x_tmp, x);            \
        ZETA_AutoVar(y_tmp, y);            \
        x_tmp < y_tmp ? x_tmp : y_tmp;     \
    })

#define ZETA_GetMinOf(x, y) \
    ZETA_GetMinOf_(x, y, ZETA_UniqueName(ZETA_tmp_), ZETA_UniqueName(ZETA_tmp_))

#define ZETA_GetMaxOf_(x, y, x_tmp, y_tmp) \
    ({                                     \
        ZETA_AutoVar(x_tmp, x);            \
        ZETA_AutoVar(y_tmp, y);            \
        x_tmp < y_tmp ? y_tmp : x_tmp;     \
    })

#define ZETA_GetMaxOf(x, y) \
    ZETA_GetMaxOf_(x, y, ZETA_UniqueName(ZETA_tmp_), ZETA_UniqueName(ZETA_tmp_))

#define ZETA_Swap_(x, y, tmp) \
    {                         \
        ZETA_AutoVar(tmp, x); \
        (x) = (y);            \
        (y) = tmp;            \
    }                         \
    ZETA_StaticAssert(TRUE)

#define ZETA_Swap(x, y) ZETA_Swap_(x, y, ZETA_UniqueName(ZETA_tmp_))

#define ZETA_GetMaxMod(type) (ZETA_RangeMaxOf(type) / 2 + 1)

ZETA_StaticAssert(ZETA_RangeMinOf(byte_t) <= 0);
ZETA_StaticAssert(255 <= ZETA_RangeMaxOf(byte_t));
