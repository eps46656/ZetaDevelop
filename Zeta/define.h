#pragma once

#include <limits.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long long int diff_t;

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

#define ZETA_MINOF(type)          \
    _Generic((type)0,             \
        char: CHAR_MIN,           \
        short: SHRT_MIN,          \
        int: INT_MIN,             \
        long int: LONG_MIN,       \
        long long int: LLONG_MIN, \
                                  \
        signed char: SCHAR_MIN,   \
                                  \
        unsigned char: 0,         \
        unsigned short: 0,        \
        unsigned int: 0,          \
        unsigned long int: 0,     \
        unsigned long long int: 0)

#define ZETA_MAXOF(type)              \
    _Generic((type)0,                 \
        char: CHAR_MAX,               \
        short: SHRT_MAX,              \
        int: INT_MAX,                 \
        long int: LONG_MAX,           \
        long long int: LLONG_MAX,     \
                                      \
        signed char: SCHAR_MAX,       \
                                      \
        unsigned char: UCHAR_MAX,     \
        unsigned short: USHRT_MAX,    \
        unsigned int: UINT_MAX,       \
        unsigned long int: ULONG_MAX, \
        unsigned long long int: ULLONG_MAX)

#define ZETA_SIGNED(type) (ZETA_MINOF(type) < 0)

#define ZETA_DECL_STRUCT(type) \
    typedef struct type type;  \
    struct type

#define ZETA_PTR_OFFSET(ptr, offset) \
    ((void*)(intptr_t)((intptr_t)(void*)(ptr) + offset))

#define ZETA_FROM_MEM(type, mem, ptr) \
    ((type*)(ZETA_ADDR_OFFSET(ptr, -offsetof(type, mem))))

#define ZETA_SWAP(x, y)      \
    {                        \
        typeof(x) tmp = (x); \
        (x) = (y);           \
        (y) = tmp;           \
    }
