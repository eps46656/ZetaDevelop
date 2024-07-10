#include <stdio.h>

#include "define.h"

#if defined(__cplusplus)
#include <iomanip>
#include <iostream>
#endif

ZETA_ExternC_Beg;

extern bool_t zeta_debugger_assert_stage;

extern void* zeta_debugger_assert_callback_context;

extern int (*zeta_debugger_assert_callback)(void* context);

#define ZETA_ImmFlush (TRUE)

#define ZETA_PrintPosFormat(filename, line, func)                             \
    printf("%48s:%-4llu\t%-24s", filename, (unsigned long long)(line), func); \
    ZETA_StaticAssert(TRUE)

#define ZETA_PrintPos                                  \
    ZETA_PrintPosFormat(__FILE__, __LINE__, __func__); \
    printf("\n");                                      \
    if (ZETA_ImmFlush) { fflush(stdout); }             \
    ZETA_StaticAssert(TRUE)

#if defined(__cplusplus)

#define ZETA_PrintVar(var)                                                  \
    ZETA_PrintPosFormat(__FILE__, __LINE__, __func__);                      \
                                                                            \
    std::cout << '\t' << std::setw(24) << ZETA_ToStr(var) << " = " << (var) \
              << '\n';

#else

#define ZETA_PrintVar(var)                             \
    ZETA_PrintPosFormat(__FILE__, __LINE__, __func__); \
                                                       \
    printf("\t%24s = ", ZETA_ToStr(var));              \
                                                       \
    printf(_Generic((var),                             \
           char: "%c\n",                               \
           unsigned char: "%X\n",                      \
           signed char: "%c\n",                        \
                                                       \
           short: "%i\n",                              \
           unsigned short: "%u\n",                     \
                                                       \
           int: "%i\n",                                \
           unsigned: "%u\n",                           \
                                                       \
           long: "%li\n",                              \
           unsigned long: "%llu\n",                    \
                                                       \
           long long: "%lli\n",                        \
           unsigned long long: "%llu\n",               \
                                                       \
           float: "%g\n",                              \
                                                       \
           double: "%g\n",                             \
           long double: "%g\n",                        \
                                                       \
           void*: "%p\n",                              \
           const void*: "%p\n",                        \
                                                       \
           char*: "%s\n",                              \
           const char*: "%s\n"),                       \
           (var));                                     \
                                                       \
    if (ZETA_ImmFlush) { fflush(stdout); }             \
    ZETA_StaticAssert(TRUE)

#endif

#define ZETA_Pause                                         \
    {                                                      \
        ZETA_PrintPosFormat(__FILE__, __LINE__, __func__); \
                                                           \
        printf("\tpause...");                              \
                                                           \
        if (FALSE) {                                       \
            fflush(stdout);                                \
            char tmp;                                      \
            scanf_s("%c", &tmp);                           \
        } else {                                           \
            printf("\n");                                  \
            if (ZETA_ImmFlush) { fflush(stdout); }         \
        }                                                  \
    }                                                      \
    ZETA_StaticAssert(TRUE)

#define ZETA_Assert(cond)                                   \
    if (cond) {                                             \
    } else {                                                \
        ZETA_PrintPosFormat(__FILE__, __LINE__, __func__);  \
                                                            \
        printf("\tassert\t%s\n", ZETA_ToStr(cond));         \
                                                            \
        if (ZETA_ImmFlush) { fflush(stdout); }              \
                                                            \
        if (!zeta_debugger_assert_stage) {                  \
            zeta_debugger_assert_stage = TRUE;              \
                                                            \
            if (zeta_debugger_assert_callback != NULL) {    \
                zeta_debugger_assert_callback(              \
                    zeta_debugger_assert_callback_context); \
            }                                               \
        };                                                  \
                                                            \
        printf("\a");                                       \
                                                            \
        exit(0);                                            \
    }                                                       \
    ZETA_StaticAssert(TRUE)

#if ZETA_IsDebug
#define ZETA_DebugAssert(cond) ZETA_Assert(cond)
#else
#define ZETA_DebugAssert(cond)
#endif

#define ZETA_CheckAssert(cond) ZETA_DebugAssert(cond)

ZETA_ExternC_End;
