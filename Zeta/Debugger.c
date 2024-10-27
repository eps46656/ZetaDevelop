#include "debugger.h"

#include "debug_str_pipe.h"
#include "pipe.h"

Zeta_Pipe* debug_pipe = NULL;

bool_t zeta_assert_stage = FALSE;

void* zeta_debugger_assert_callback_context = NULL;

int (*zeta_debugger_assert_callback)(void* context) = NULL;

void Zeta_Debugger_InitPipe() {
    if (debug_pipe == NULL) {
        debug_pipe = malloc(sizeof(Zeta_Pipe));
        Zeta_DebugStrPipe_DeployPipe(Zeta_DebugStrPipe_Create(), debug_pipe);
    }
}

void Zeta_Debugger_ClearPipe() {
    if (debug_pipe != NULL) { debug_pipe->Clear(debug_pipe->context); }
}

void Zeta_Debugger_FlushPipe() {
    if (debug_pipe == NULL) { return; }

    unsigned char c;

    while (!debug_pipe->IsEmpty(debug_pipe->context)) {
        debug_pipe->Read(debug_pipe->context, 1, &c);
        printf("%c", c);
    }
}
