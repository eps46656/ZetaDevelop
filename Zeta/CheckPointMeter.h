#pragma once

#include "Debugger.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_CheckPointMeter);

void Zeta_CheckPointMeter_Create(void* cpr);

void Zeta_CheckPointMeter_Destroy(void* cpr);

void Zeta_CheckPointMeter_Record(void* cpr, char const* filename, size_t line,
                                 char const* func);

void Zeta_CheckPointMeter_Clear(void* cpr);

void Zeta_CheckPointMeter_Print(void* cpr);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define ZETA_RecordCheckPoint(cpr) \
    Zeta_CheckPointMeter_Record(cpr, __FILE__, __LINE__, __func__)

#define ZETA_RecordStdCheckPoint   \
    Zeta_InitStdCheckPointMeter(); \
    ZETA_RecordCheckPoint(std_check_point_recorder)

extern void* std_check_point_recorder;

void Zeta_InitStdCheckPointMeter();

ZETA_ExternC_End;
