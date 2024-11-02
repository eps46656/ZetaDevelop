#include <sanitizer/common_interface_defs.h>
#include <stdio.h>

#include "logger.h"

ZETA_ExternC_Beg;

extern Zeta_Pipe* zeta_debug_pipe;

extern bool_t zeta_assert_stage;

#define ZETA_Debug_ImmPrint FALSE

#define ZETA_DebugLogCurPos                        \
    Zeta_Debugger_InitPipe();                      \
    if (ZETA_Debug_ImmPrint) { ZETA_PrintCurPos; } \
    ZETA_LogCurPos(zeta_debug_pipe)

#define ZETA_DebugLogVar(var)                        \
    Zeta_Debugger_InitPipe();                        \
    if (ZETA_Debug_ImmPrint) { ZETA_PrintVar(var); } \
    ZETA_LogVar(zeta_debug_pipe, var)

#define ZETA_Pause                                              \
    {                                                           \
        ZETA_PrintPos(__FILE__, __LINE__, __PRETTY_FUNCTION__); \
                                                                \
        printf("\tpause...");                                   \
                                                                \
        if (TRUE) {                                             \
            fflush(stdout);                                     \
            char tmp;                                           \
            scanf_s("%c", &tmp);                                \
        } else {                                                \
            printf("\n");                                       \
            if (ZETA_ImmPrint) { fflush(stdout); }              \
        }                                                       \
    }                                                           \
    ZETA_StaticAssert(TRUE)

#if __has_feature(address_sanitizer)

#define ZETA_PrintStackTrace __sanitizer_print_stack_trace()

#else

#define ZETA_PrintStackTrace ZETA_StaticAssert(TRUE)

#endif

#define ZETA_AssertVerbose(pipe, cond, callback, callback_context)             \
    if (cond) {                                                                \
    } else {                                                                   \
        if (!zeta_assert_stage) {                                              \
            zeta_assert_stage = TRUE;                                          \
                                                                               \
            ZETA_LogPos(pipe, __FILE__, __LINE__, __PRETTY_FUNCTION__, FALSE); \
                                                                               \
            printf("\tassert\t%s\n", ZETA_ToStr(cond));                        \
                                                                               \
            if (ZETA_ImmPrint) { fflush(stdout); }                             \
                                                                               \
            callback(callback_context);                                        \
                                                                               \
            printf("\a");                                                      \
        }                                                                      \
                                                                               \
        ZETA_PrintStackTrace;                                                  \
                                                                               \
        exit(1);                                                               \
    }                                                                          \
    ZETA_StaticAssert(TRUE)

#if ZETA_IsDebug
#define ZETA_DebugAssert(cond)                                      \
    {                                                               \
        Zeta_Debugger_InitPipe();                                   \
                                                                    \
        if (cond) {                                                 \
        } else {                                                    \
            if (!zeta_assert_stage) {                               \
                zeta_assert_stage = TRUE;                           \
                                                                    \
                ZETA_PrintCurPos;                                   \
                printf("\033[31mdebug assert\t%s\033[0m\n", #cond); \
                fflush(stdout);                                     \
                                                                    \
                Zeta_Debugger_FlushPipe();                          \
                                                                    \
                ZETA_PrintCurPos;                                   \
                printf("\033[31mdebug assert\t%s\033[0m\n", #cond); \
                printf("\a");                                       \
                fflush(stdout);                                     \
            }                                                       \
                                                                    \
            ZETA_PrintStackTrace;                                   \
                                                                    \
            exit(1);                                                \
        }                                                           \
    }                                                               \
    ZETA_StaticAssert(TRUE)

#else
#define ZETA_DebugAssert(cond)
#endif

#define ZETA_CheckAssert(cond) ZETA_DebugAssert(cond)

void Zeta_Debugger_InitPipe();

void Zeta_Debugger_ClearPipe();

void Zeta_Debugger_FlushPipe();

ZETA_ExternC_End;
