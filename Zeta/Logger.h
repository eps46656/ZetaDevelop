#pragma once

#include "Debugger.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_Logger);

void Zeta_Logger_Create(void* cpr);

void Zeta_Logger_Destroy(void* cpr);

void Zeta_Logger_Log(void* logger, char const* filename, size_t line,
                     char const* func, char const* msg);

void Zeta_Logger_Clear(void* cpr);

void Zeta_Logger_Print(void* cpr);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define ZETA_Logger_Log(logger, msg) \
    Zeta_Logger_Log(logger, __FILE__, __LINE__, __func__, msg)

#define ZETA_DebugLog(msg)  \
    Zeta_InitDebugLogger(); \
    ZETA_Logger_Log(debug_logger, msg)

#define ZETA_DebugLogPos ZETA_DebugLog(NULL)

extern void* debug_logger;

void Zeta_InitDebugLogger();

ZETA_ExternC_End;
