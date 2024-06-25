#pragma once

#include "Debugger.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_CheckPointRecorder);

void Zeta_CheckPointRecorder_Create(void* cpr);

void Zeta_CheckPointRecorder_Destroy(void* cpr);

void Zeta_CheckPointRecorder_Record(void* cpr, char const* filename,
                                    size_t line, char const* func);

void Zeta_CheckPointRecorder_Print(void* cpr);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define ZETA_RecordCheckPoint(cpr) \
    Zeta_CheckPointRecorder_Record(cpr, __FILE__, __LINE__, __func__)

#define ZETA_RecordStdCheckPoint      \
    Zeta_InitStdCheckPointRecorder(); \
    ZETA_RecordCheckPoint(std_check_point_recorder)

extern void* std_check_point_recorder;

void Zeta_InitStdCheckPointRecorder();

ZETA_ExternC_End;
