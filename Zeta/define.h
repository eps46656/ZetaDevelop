#pragma once

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__cplusplus)
#define bool_t bool
#else
#define bool_t _Bool
#endif

#define TRUE ((bool_t)(0 == 0))
#define FALSE ((bool_t)(0 != 0))

#define ZETA_StaticAssert(cond) _Static_assert(cond, "")

#if !defined(ZETA_IsDebug)
#if defined(DEBUG)
#define ZETA_IsDebug 1
#else
#define ZETA_IsDebug 0
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

typedef unsigned unichar_t;

#define ZETA_Unused(x)          \
    if (FALSE) { ((void)(x)); } \
    ZETA_StaticAssert(TRUE)

#define ZETA_ToStr_(x) #x
#define ZETA_ToStr(x) ZETA_ToStr_(x)

#define ZETA_Concat_(x, y) x##y
#define ZETA_Concat(x, y) ZETA_Concat_(x, y)

#define ZETA_UniqueName(prefix) ZETA_Concat(prefix, __COUNTER__)

#define ZETA_TmpName ZETA_UniqueName(ZETA_tmp_)

#define ZETA_DeclareStruct(struct_name)     \
    typedef struct struct_name struct_name; \
    struct struct_name;                     \
    ZETA_StaticAssert(TRUE)

#if defined(__cplusplus)

#define ZETA_TypeOf(expression) decltype(expression)

#else

#define ZETA_TypeOf(expression) typeof(expression)

#endif

#if defined(__cplusplus)
#define ZETA_AutoVar(var, expression) \
    auto var{ (expression) };         \
    ZETA_StaticAssert(TRUE)
#else
#define ZETA_AutoVar(var, expression)             \
    ZETA_TypeOf((expression)) var = (expression); \
    ZETA_StaticAssert(TRUE)
#endif

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

#if defined(__cplusplus)

#define ZETA_ToVoidPtr(ptr) (const_cast<void*>((void const*)(ptr)))

#else

#define ZETA_ToVoidPtr(ptr) ((void*)(ptr))

#endif

#define ZETA_IsSigned(type)        \
    _Generic((type)0,              \
        char: ((char)(-1) < 0),    \
                                   \
        unsigned char: FALSE,      \
        unsigned short: FALSE,     \
        unsigned: FALSE,           \
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
        unsigned: ZETA_UIntRangeMin_(unsigned, signed int),               \
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
        signed int: ZETA_SIntRangeMin_(unsigned, signed int),             \
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
        unsigned: ZETA_UIntRangeMax_(unsigned, signed int),               \
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
        signed int: ZETA_SIntRangeMax_(unsigned, signed int),             \
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
        unsigned: __builtin_popcount(ZETA_RangeMaxOf(unsigned)),             \
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

typedef unsigned _BitInt(ZETA_WidthOf(unsigned long long)) ULLBitInt_t;

#define ZETA_InRangeOf_(tmp_val, val, type)                          \
    ({                                                               \
        ZETA_AutoVar(tmp_val, val);                                  \
        ZETA_RangeMinOf(type) <= val&& val <= ZETA_RangeMaxOf(type); \
    })

#define ZETA_InRangeOf(val, type) ZETA_InRangeOf_(ZETA_TmpName, val, type)

#define ZETA_2Power(x) ((ULLBitInt_t)(1) << (x))

#define ZETA_Is2Power(x) (__builtin_popcountll(x) == 1)

#define ZETA_FloorLog2(x) \
    (ZETA_WidthOf(unsigned long long) - 1 - __builtin_clzll(x))

#define ZETA_CeilLog2(x) \
    (ZETA_WidthOf(unsigned long long) - __builtin_clzll(x - 1))

#define ZETA_PtrToAddr(x) ((uintptr_t)(void const*)(x))
#define ZETA_AddrToPtr(x) ((void*)(uintptr_t)(x))

#define ZETA_MemberToStruct(struct_type, member_name, member_ptr) \
    ((struct_type*)((unsigned char*)(member_ptr) -                \
                    offsetof(struct_type, member_name)))

#define ZETA_GetMinOf_(tmp_x, tmp_y, x, y) \
    ({                                     \
        ZETA_AutoVar(tmp_x, x);            \
        ZETA_AutoVar(tmp_y, y);            \
        tmp_x <= tmp_y ? tmp_x : tmp_y;    \
    })

#define ZETA_GetMinOf(x, y) ZETA_GetMinOf_(ZETA_TmpName, ZETA_TmpName, (x), (y))

#define ZETA_GetMaxOf_(tmp_x, tmp_y, x, y) \
    ({                                     \
        ZETA_AutoVar(tmp_x, x);            \
        ZETA_AutoVar(tmp_y, y);            \
        tmp_x < tmp_y ? tmp_y : tmp_x;     \
    })

#define ZETA_GetMaxOf(x, y) ZETA_GetMaxOf_(ZETA_TmpName, ZETA_TmpName, (x), (y))

#define ZETA_ThreeWayCompare_(tmp_x, tmp_y, x, y) \
    ({                                            \
        ZETA_AutoVar(tmp_x, x);                   \
        ZETA_AutoVar(tmp_y, y);                   \
        (tmp_y < tmp_x) - (tmp_x < tmp_y);        \
    })

#define ZETA_ThreeWayCompare(x, y) \
    ZETA_ThreeWayCompare_(ZETA_TmpName, ZETA_TmpName, x, y)

#define ZETA_Swap_(tmp_x, tmp_y, x, y, tmp) \
    {                                       \
        ZETA_AutoVar(tmp_x, &(x));          \
        ZETA_AutoVar(tmp_y, &(y));          \
        ZETA_AutoVar(tmp, *tmp_x);          \
        *tmp_x = *tmp_y;                    \
        *tmp_y = tmp;                       \
    }                                       \
    ZETA_StaticAssert(TRUE)

#define ZETA_Swap(x, y) \
    ZETA_Swap_(ZETA_TmpName, ZETA_TmpName, (x), (y), ZETA_TmpName)

#define ZETA_GetMaxMod(type) (ZETA_RangeMaxOf(type) / 2 + 1)

ZETA_StaticAssert(ZETA_RangeMinOf(byte_t) <= 0);

ZETA_StaticAssert(255 <= ZETA_RangeMaxOf(byte_t));

#define ZETA_ModAddInv_(tmp_mod, val, mod)             \
    ({                                                 \
        ZETA_AutoVar(tmp_mod, (mod));                  \
        tmp_mod - 1 - ((val) + tmp_mod - 1) % tmp_mod; \
    })

#define ZETA_ModAddInv(val, mod) ZETA_ModAddInv_(ZETA_TmpName, (val), (mod))

#define ZETA_CeilIntDiv_(tmp_x, tmp_y, x, y) \
    ({                                       \
        ZETA_AutoVar(tmp_x, x);              \
        ZETA_AutoVar(tmp_y, y);              \
        (tmp_x + tmp_y - 1) / tmp_y;         \
    })

#define ZETA_CeilIntDiv(x, y) \
    ZETA_CeilIntDiv_(ZETA_TmpName, ZETA_TmpName, (x), (y))

#define ZETA_RoundIntDiv_(tmp_x, tmp_y, x, y) \
    ({                                        \
        ZETA_AutoVar(tmp_x, x);               \
        ZETA_AutoVar(tmp_y, y);               \
        (tmp_x + tmp_y / 2) / tmp_y;          \
    })

#define ZETA_RoundIntDiv(x, y) \
    ZETA_RoundIntDiv_(ZETA_TmpName, ZETA_TmpName, (x), (y))

#define ZETA_FixedPoint_BaseOrder (24)

#define ZETA_FixedPoint_Base ZETA_2Power(ZETA_FixedPoint_BaseOrder)

#define ZETA_AreOverlapped_(tmp_a_beg, tmp_a_end, tmp_b_beg, tmp_b_end, a_beg, \
                            a_size, b_beg, b_size)                             \
    ({                                                                         \
        void const* tmp_a_beg = (a_beg);                                       \
        void const* tmp_a_end =                                                \
            ZETA_AddrToPtr(ZETA_PtrToAddr(tmp_a_beg) + (a_size));              \
        void const* tmp_b_beg = (b_beg);                                       \
        void const* tmp_b_end =                                                \
            ZETA_AddrToPtr(ZETA_PtrToAddr(tmp_b_beg) + (b_size));              \
        (tmp_b_beg < tmp_a_end) && (tmp_a_beg < tmp_b_end);                    \
    })

#define ZETA_AreOverlapped(a_beg, a_size, b_beg, b_size)          \
    ZETA_AreOverlapped_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, \
                        ZETA_TmpName, (a_beg), (a_size), (b_beg), (b_size))

#define ZETA_ImmPrint (TRUE)

#define ZETA_PrintPos(filename, line, func)                              \
    printf("%48s:%-4llu\t%-24s", (filename), (unsigned long long)(line), \
           (func));                                                      \
    ZETA_StaticAssert(TRUE)

#define ZETA_PrintCurPos                                    \
    ZETA_PrintPos(__FILE__, __LINE__, __PRETTY_FUNCTION__); \
    printf("\n");                                           \
    if (ZETA_ImmPrint) { fflush(stdout); }                  \
    ZETA_StaticAssert(TRUE)

#define ZETA_PrintVar(var)                                      \
    {                                                           \
        ZETA_PrintPos(__FILE__, __LINE__, __PRETTY_FUNCTION__); \
                                                                \
        printf("\t%24s = ", ZETA_ToStr(var));                   \
                                                                \
        printf(_Generic((var),                                  \
               bool_t: "%c\n",                                  \
                                                                \
               char: "%c\n",                                    \
               unsigned char: "%X\n",                           \
               signed char: "%c\n",                             \
                                                                \
               short: "%i\n",                                   \
               unsigned short: "%u\n",                          \
                                                                \
               int: "%i\n",                                     \
               unsigned: "%u\n",                                \
                                                                \
               long: "%li\n",                                   \
               unsigned long: "%lu\n",                          \
                                                                \
               long long: "%lli\n",                             \
               unsigned long long: "%llu\n",                    \
                                                                \
               float: "%g\n",                                   \
                                                                \
               double: "%g\n",                                  \
               long double: "%g\n",                             \
                                                                \
               void*: "%p\n",                                   \
               const void*: "%p\n",                             \
                                                                \
               char*: "%s\n",                                   \
               const char*: "%s\n"),                            \
               (var));                                          \
                                                                \
        if (ZETA_ImmPrint) { fflush(stdout); }                  \
    }                                                           \
    ZETA_StaticAssert(TRUE)

#define ZETA_CallMemberFunc__(tmp_obj, member_func, ...) \
    ZETA_DebugAssert(tmp_obj != NULL);                   \
    ZETA_DebugAssert(tmp_obj->member_func != NULL);      \
    tmp_obj->member_func(__VA_ARGS__);

#define ZETA_CallMemberFunc_(tmp_obj, obj, member_func, ...)          \
    ({                                                                \
        ZETA_AutoVar(tmp_obj, (obj));                                 \
        ZETA_CallMemberFunc__(tmp_obj, member_func, tmp_obj->context, \
                              __VA_ARGS__)                            \
    })

#define ZETA_CallMemberFunc(obj, member_func, ...) \
    ZETA_CallMemberFunc_(ZETA_TmpName, obj, member_func, __VA_ARGS__)

typedef int (*Zeta_Compare)(void* context, void const* a, void const* b);

typedef unsigned long long (*Zeta_Hash)(void* context, void const* a,
                                        unsigned long long salt);

typedef void (*Zeta_FuncPtr_ArrayRead)(void* context, void const* data,
                                       size_t width, size_t stride,
                                       size_t size);

typedef void (*Zeta_FuncPtr_ArrayWrite)(void* context, void* data, size_t width,
                                        size_t stride, size_t size);
