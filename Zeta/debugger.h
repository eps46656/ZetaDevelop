#include <sanitizer/common_interface_defs.h>
#include <stdio.h>

#include "logger.h"

ZETA_ExternC_Beg;

extern Zeta_Pipe* zeta_debug_pipe;

extern bool_t zeta_assert_stage;

#define ZETA_Debug_ImmPrint FALSE

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

#define ZETA_Assert(cond)                                     \
    {                                                         \
        Zeta_Debugger_InitPipe();                             \
                                                              \
        if (cond) {                                           \
        } else {                                              \
            if (!zeta_assert_stage) {                         \
                zeta_assert_stage = TRUE;                     \
                                                              \
                ZETA_PrintCurPos;                             \
                printf("\033[31massert\t%s\033[0m\n", #cond); \
                fflush(stdout);                               \
                                                              \
                Zeta_Debugger_FlushPipe();                    \
                                                              \
                ZETA_PrintCurPos;                             \
                printf("\033[31massert\t%s\033[0m\n", #cond); \
                printf("\a");                                 \
            }                                                 \
                                                              \
            ZETA_PrintStackTrace;                             \
                                                              \
            if (ZETA_ImmPrint) { fflush(stdout); }            \
                                                              \
            exit(1);                                          \
        }                                                     \
    }                                                         \
    ZETA_StaticAssert(TRUE)

#define ZETA_CheckAssert(cond) ZETA_DebugAssert(cond)

#if ZETA_EnableArguDebug

#define ZETA_ArugAssert(cond) ZETA_Assert((cond))

#else

#define ZETA_ArugAssert(cond) ZETA_Unused((cond))

#endif

#if ZETA_EnableIntegrityDebug

#define ZETA_IntegrityAssert(cond) ZETA_Assert((cond))

#else

#define ZETA_IntegrityAssert(cond) ZETA_Unused((cond))

#endif

#if ZETA_EnableDebug

#define ZETA_DebugAssert(cond) ZETA_Assert((cond))

#else

#define ZETA_DebugAssert(cond) ZETA_Unused((cond))

#endif

void Zeta_Debugger_InitPipe();

void Zeta_Debugger_ClearPipe();

void Zeta_Debugger_FlushPipe();

ZETA_ExternC_End;
