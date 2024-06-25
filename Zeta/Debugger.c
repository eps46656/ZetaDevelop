#include "Debugger.h"

bool_t zeta_debugger_assert_stage = FALSE;

void* zeta_debugger_assert_callback_context = NULL;

int (*zeta_debugger_assert_callback)(void* context) = NULL;
