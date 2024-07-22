#include <stdio.h>

#include "Logger.h"

ZETA_ExternC_Beg;

extern Zeta_Pipe* debug_pipe;

extern bool_t zeta_assert_stage;

#define ZETA_Debug_ImmPrint FALSE

#define ZETA_DebugLogCurPos                        \
    Zeta_Debugger_InitPipe();                      \
    if (ZETA_Debug_ImmPrint) { ZETA_PrintCurPos; } \
    ZETA_LogCurPos(debug_pipe)

#define ZETA_DebugLogVar(var)                        \
    Zeta_Debugger_InitPipe();                        \
    if (ZETA_Debug_ImmPrint) { ZETA_PrintVar(var); } \
    ZETA_LogVar(debug_pipe, var)

#define ZETA_Pause                                   \
    {                                                \
        ZETA_PrintPos(__FILE__, __LINE__, __func__); \
                                                     \
        printf("\tpause...");                        \
                                                     \
        if (TRUE) {                                  \
            fflush(stdout);                          \
            char tmp;                                \
            scanf_s("%c", &tmp);                     \
        } else {                                     \
            printf("\n");                            \
            if (ZETA_ImmPrint) { fflush(stdout); }   \
        }                                            \
    }                                                \
    ZETA_StaticAssert(TRUE)

#define ZETA_AssertVerbose(pipe, cond, callback, callback_context)  \
    if (cond) {                                                     \
    } else {                                                        \
        if (!zeta_assert_stage) {                                   \
            zeta_assert_stage = TRUE;                               \
                                                                    \
            ZETA_LogPos(pipe, __FILE__, __LINE__, __func__, FALSE); \
                                                                    \
            printf("\tassert\t%s\n", ZETA_ToStr(cond));             \
                                                                    \
            if (ZETA_ImmPrint) { fflush(stdout); }                  \
                                                                    \
            callback(callback_context);                             \
                                                                    \
            printf("\a");                                           \
        }                                                           \
                                                                    \
        exit(0);                                                    \
    }                                                               \
    ZETA_StaticAssert(TRUE)

#if ZETA_IsDebug
#define ZETA_DebugAssert(cond)                                  \
    {                                                           \
        Zeta_Debugger_InitPipe();                               \
                                                                \
        if (cond) {                                             \
        } else {                                                \
            if (!zeta_assert_stage) {                           \
                zeta_assert_stage = TRUE;                       \
                                                                \
                ZETA_PrintCurPos;                               \
                printf("debug assert\t%s\n", ZETA_ToStr(cond)); \
                fflush(stdout);                                 \
                                                                \
                Zeta_Debugger_FlushPipe();                      \
                                                                \
                ZETA_PrintCurPos;                               \
                printf("debug assert\t%s\n", ZETA_ToStr(cond)); \
                printf("\a");                                   \
                fflush(stdout);                                 \
            }                                                   \
                                                                \
            exit(0);                                            \
        }                                                       \
    }                                                           \
    ZETA_StaticAssert(TRUE)

#else
#define ZETA_DebugAssert(cond)
#endif

#define ZETA_CheckAssert(cond) ZETA_DebugAssert(cond)

void Zeta_Debugger_InitPipe();

void Zeta_Debugger_ClearPipe();

void Zeta_Debugger_FlushPipe();

ZETA_ExternC_End;
