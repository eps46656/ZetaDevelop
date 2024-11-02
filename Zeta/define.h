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

#define ZETA_IsSigned(type) ((type)(-1) < 0)

// -----------------------------------------------------------------------------

#define ZETA_UIntRangeMin_(type) ((unsigned type)(0))

#define ZETA_UIntRangeMax_(type) ((unsigned type)(-1))

#define ZETA_SIntRangeMin_(type)                                              \
    ((signed type)(-(signed type)((unsigned type)(-1) / (unsigned type)(2)) - \
                   (signed type)(1)))

#define ZETA_SIntRangeMax_(type) ((signed type)((unsigned type)(-1) / 2))

// ---

#define ZETA_CHAR_MIN (ZETA_IsSigned(char) ? ZETA_SCHAR_MIN : ZETA_UCHAR_MIN)
#define ZETA_UCHAR_MIN ZETA_UIntRangeMin_(char)
#define ZETA_SCHAR_MIN ZETA_SIntRangeMin_(char)

#define ZETA_USHRT_MIN ZETA_UIntRangeMin_(short)
#define ZETA_SSHRT_MIN ZETA_SIntRangeMin_(short)

#define ZETA_UINT_MIN ZETA_UIntRangeMin_(int)
#define ZETA_SINT_MIN ZETA_SIntRangeMin_(int)

#define ZETA_ULONG_MIN ZETA_UIntRangeMin_(int)
#define ZETA_SLONG_MIN ZETA_SIntRangeMin_(long)

#define ZETA_ULLONG_MIN ZETA_UIntRangeMin_(long long)
#define ZETA_SLLONG_MIN ZETA_SIntRangeMin_(long long)

#define ZETA_U8_MIN ZETA_UIntRangeMin_(_BitInt(8))
#define ZETA_S8_MIN ZETA_SIntRangeMin_(_BitInt(8))

#define ZETA_U16_MIN ZETA_SIntRangeMin_(_BitInt(16))
#define ZETA_S16_MIN ZETA_SIntRangeMin_(_BitInt(16))

#define ZETA_U32_MIN ZETA_SIntRangeMin_(_BitInt(32))
#define ZETA_S32_MIN ZETA_SIntRangeMin_(_BitInt(32))

#define ZETA_U64_MIN ZETA_SIntRangeMin_(_BitInt(64))
#define ZETA_S64_MIN ZETA_SIntRangeMin_(_BitInt(64))

#define ZETA_U128_MIN ZETA_SIntRangeMin_(_BitInt(128))
#define ZETA_S128_MIN ZETA_SIntRangeMin_(_BitInt(128))

// ---

#define ZETA_CHAR_MAX (ZETA_IsSigned(char) ? ZETA_SCHAR_MAX : ZETA_UCHAR_MAX)
#define ZETA_UCHAR_MAX ZETA_UIntRangeMax_(char)
#define ZETA_SCHAR_MAX ZETA_SIntRangeMax_(char)

#define ZETA_USHRT_MAX ZETA_UIntRangeMax_(short)
#define ZETA_SSHRT_MAX ZETA_SIntRangeMax_(short)

#define ZETA_UINT_MAX ZETA_UIntRangeMax_(int)
#define ZETA_SINT_MAX ZETA_SIntRangeMax_(int)

#define ZETA_ULONG_MAX ZETA_UIntRangeMax_(long)
#define ZETA_SLONG_MAX ZETA_SIntRangeMax_(long)

#define ZETA_ULLONG_MAX ZETA_UIntRangeMax_(long long)
#define ZETA_SLLONG_MAX ZETA_SIntRangeMax_(long long)

#define ZETA_U8_MAX ZETA_UIntRangeMax_(_BitInt(8))
#define ZETA_S8_MAX ZETA_SIntRangeMax_(_BitInt(8))

#define ZETA_U16_MAX ZETA_UIntRangeMax_(_BitInt(16))
#define ZETA_S16_MAX ZETA_SIntRangeMax_(_BitInt(16))

#define ZETA_U32_MAX ZETA_UIntRangeMax_(_BitInt(32))
#define ZETA_S32_MAX ZETA_SIntRangeMax_(_BitInt(32))

#define ZETA_U64_MAX ZETA_UIntRangeMax_(_BitInt(64))
#define ZETA_S64_MAX ZETA_SIntRangeMax_(_BitInt(64))

#define ZETA_U128_MAX ZETA_UIntRangeMax_(_BitInt(128))
#define ZETA_S128_MAX ZETA_SIntRangeMax_(_BitInt(128))

// -----------------------------------------------------------------------------

#define ZETA_RangeMinOf(type)                \
    _Generic((type)0,                        \
        char: ZETA_CHAR_MIN,                 \
        unsigned char: ZETA_UCHAR_MIN,       \
        signed char: ZETA_SCHAR_MIN,         \
                                             \
        unsigned short: ZETA_USHRT_MIN,      \
        signed short: ZETA_SSHRT_MIN,        \
                                             \
        unsigned: ZETA_UINT_MIN,             \
        signed: ZETA_SINT_MIN,               \
                                             \
        unsigned long: ZETA_ULONG_MIN,       \
        signed long: ZETA_SLONG_MIN,         \
                                             \
        unsigned long long: ZETA_ULLONG_MIN, \
        signed long long: ZETA_SLLONG_MIN,   \
                                             \
        u8_t: ZETA_U8_MIN,                   \
        s8_t: ZETA_S8_MIN,                   \
                                             \
        u16_t: ZETA_U16_MIN,                 \
        s16_t: ZETA_S16_MIN,                 \
                                             \
        u32_t: ZETA_U32_MIN,                 \
        s32_t: ZETA_S32_MIN,                 \
                                             \
        u64_t: ZETA_U64_MIN,                 \
        s64_t: ZETA_S64_MIN,                 \
                                             \
        u128_t: ZETA_U128_MIN,               \
        s128_t: ZETA_S128_MIN)

#define ZETA_RangeMaxOf(type)                \
    _Generic((type)0,                        \
        char: ZETA_CHAR_MAX,                 \
        unsigned char: ZETA_UCHAR_MAX,       \
        signed char: ZETA_SCHAR_MAX,         \
                                             \
        unsigned short: ZETA_USHRT_MAX,      \
        signed short: ZETA_SSHRT_MAX,        \
                                             \
        unsigned: ZETA_UINT_MAX,             \
        signed: ZETA_SINT_MAX,               \
                                             \
        unsigned long: ZETA_ULONG_MAX,       \
        signed long: ZETA_SLONG_MAX,         \
                                             \
        unsigned long long: ZETA_ULLONG_MAX, \
        signed long long: ZETA_SLLONG_MAX,   \
                                             \
        u8_t: ZETA_U8_MAX,                   \
        s8_t: ZETA_S8_MAX,                   \
                                             \
        u16_t: ZETA_U16_MAX,                 \
        s16_t: ZETA_S16_MAX,                 \
                                             \
        u32_t: ZETA_U32_MAX,                 \
        s32_t: ZETA_S32_MAX,                 \
                                             \
        u64_t: ZETA_U64_MAX,                 \
        s64_t: ZETA_S64_MAX,                 \
                                             \
        u128_t: ZETA_U128_MAX,               \
        s128_t: ZETA_S128_MAX)

#define ZETA_UCHAR_WIDTH (__CHAR_BIT__)
#define ZETA_USHRT_WIDTH (__SHRT_WIDTH__)
#define ZETA_UINT_WIDTH (__INT_WIDTH__)
#define ZETA_ULONG_WIDTH (__LONG_WIDTH__)
#define ZETA_ULLONG_WIDTH (__LLONG_WIDTH__)
#define ZETA_U8_WIDTH (8)
#define ZETA_U16_WIDTH (16)
#define ZETA_U32_WIDTH (32)
#define ZETA_U64_WIDTH (64)
#define ZETA_U128_WIDTH (128)

#define ZETA_WidthOf(type)                     \
    _Generic((type)0,                          \
        unsigned char: ZETA_UCHAR_WIDTH,       \
        unsigned short: ZETA_USHRT_WIDTH,      \
        unsigned: ZETA_UINT_WIDTH,             \
        unsigned long: ZETA_ULONG_WIDTH,       \
        unsigned long long: ZETA_ULLONG_WIDTH, \
        u8_t: ZETA_U8_WIDTH,                   \
        u16_t: ZETA_U16_WIDTH,                 \
        u32_t: ZETA_U32_WIDTH,                 \
        u64_t: ZETA_U64_WIDTH,                 \
        u128_t: ZETA_U128_WIDTH)

typedef unsigned _BitInt(ZETA_WidthOf(unsigned long long)) ULLBitInt_t;

#define ZETA_InRangeOf_(tmp_val, val, type)                          \
    ({                                                               \
        ZETA_AutoVar(tmp_val, val);                                  \
        ZETA_RangeMinOf(type) <= val&& val <= ZETA_RangeMaxOf(type); \
    })

#define ZETA_InRangeOf(val, type) ZETA_InRangeOf_(ZETA_TmpName, (val), type)

#define ZETA_2Power(x) ((ULLBitInt_t)(1) << (x))

#define ZETA_Is2Power(x) (__builtin_popcountll(x) == 1)

#define ZETA_FloorLog2_(tmp_x, x)                                         \
    ({                                                                    \
        ZETA_AutoVar(tmp_x, x);                                           \
        tmp_x == 0 ? -1 : ZETA_ULLONG_WIDTH - 1 - __builtin_clzll(tmp_x); \
    })

#define ZETA_FloorLog2(x) ZETA_FloorLog2_(ZETA_TmpName, (x))

#define ZETA_CeilLog2_(tmp_x, x)                                  \
    ({                                                            \
        ZETA_AutoVar(tmp_x, x);                                   \
        tmp_x == 0 ? -1                                           \
                   : ZETA_ULLONG_WIDTH - __builtin_clzll(tmp_x) - \
                         (__builtin_popcountll(tmp_x) == 1);      \
    })

#define ZETA_CeilLog2(x) ZETA_CeilLog2_(ZETA_TmpName, (x))

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
    ZETA_ThreeWayCompare_(ZETA_TmpName, ZETA_TmpName, (x), (y))

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

#define ZETA_FixedPoint_BaseOrder (ZETA_ULLONG_WIDTH * 3 / 8)

#define ZETA_FixedPoint_Base ZETA_2Power(ZETA_FixedPoint_BaseOrder)

#define ZETA_AreOverlapped_(tmp_a_beg, tmp_a_end, tmp_b_beg, tmp_b_end, a_beg, \
                            a_size, b_beg, b_size)                             \
    ({                                                                         \
        unsigned char const* tmp_a_beg = (void const*)(a_beg);                 \
        unsigned char const* tmp_a_end = tmp_a_beg + (a_size);                 \
        unsigned char const* tmp_b_beg = (void const*)(b_beg);                 \
        unsigned char const* tmp_b_end = tmp_b_beg + (b_size);                 \
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

#define ZETA_LittleEndian (0)
#define ZETA_BigEndian (1)

typedef int (*Zeta_Compare)(void* context, void const* a, void const* b);

typedef unsigned long long (*Zeta_Hash)(void* context, void const* a,
                                        unsigned long long salt);

typedef void (*Zeta_FuncPtr_ArrayRead)(void* context, void const* data,
                                       size_t width, size_t stride,
                                       size_t size);

typedef void (*Zeta_FuncPtr_ArrayWrite)(void* context, void* data, size_t width,
                                        size_t stride, size_t size);
