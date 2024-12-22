#include "debugger.h"

#include "debug_str_pipe.h"
#include "pipe.h"

Zeta_Pipe* zeta_debug_pipe = NULL;

bool_t zeta_assert_stage = FALSE;

void* zeta_debugger_assert_callback_context = NULL;

int (*zeta_debugger_assert_callback)(void* context) = NULL;

void Zeta_Debugger_InitPipe() {
    if (zeta_debug_pipe == NULL) {
        zeta_debug_pipe = malloc(sizeof(Zeta_Pipe));
        Zeta_DebugStrPipe_DeployPipe(Zeta_DebugStrPipe_Create(),
                                     zeta_debug_pipe);
    }
}

void Zeta_Debugger_ClearPipe() {
    if (zeta_debug_pipe != NULL) {
        zeta_debug_pipe->Clear(zeta_debug_pipe->context);
    }
}

void Zeta_Debugger_FlushPipe() {
    if (zeta_debug_pipe == NULL) { return; }

    unsigned char c;

    while (!zeta_debug_pipe->IsEmpty(zeta_debug_pipe->context)) {
        ZETA_CallMemberFunc(zeta_debug_pipe, Read, 1, &c);
        printf("%c", c);
    }
}
