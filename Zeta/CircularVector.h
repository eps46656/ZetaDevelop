#pragma once

#include "Cursor.h"
#include "DebugTreeMap.h"
#include "SeqContainer.h"

ZETA_ExternC_Beg;

#define ZETA_CircularVector_max_capacity (ZETA_GetRangeMax(size_t) / 2 + 1)

typedef struct Zeta_CircularVector Zeta_CircularVector;

struct Zeta_CircularVector {
    size_t width;
    size_t stride;
    size_t offset;
    size_t size;
    size_t capacity;
    void* data;
};

typedef struct Zeta_CircularVector_Cursor Zeta_CircularVector_Cursor;

struct Zeta_CircularVector_Cursor {
    size_t idx;
};

void Zeta_CircularVector_Init(void* cv);

size_t Zeta_CircularVector_GetWidth(void* cv);

size_t Zeta_CircularVector_GetStride(void* cv);

size_t Zeta_CircularVector_GetOffset(void* cv);

size_t Zeta_CircularVector_GetSize(void* cv);

size_t Zeta_CircularVector_GetCapacity(void* cv);

void Zeta_CircularVector_GetLBCursor(void* cv, void* dst_cursor);

void Zeta_CircularVector_GetRBCursor(void* cv, void* dst_cursor);

void* Zeta_CircularVector_PeekL(void* cv, void* dst_cursor);

void* Zeta_CircularVector_PeekR(void* cv, void* dst_cursor);

void* Zeta_CircularVector_Access(void* cv, void* dst_cursor, size_t idx);

void* Zeta_CircularVector_PushL(void* cv, void* dst_cursor);

void* Zeta_CircularVector_PushR(void* cv, void* dst_cursor);

void* Zeta_CircularVector_Insert(void* cv, void* pos_cursor);

void Zeta_CircularVector_PopL(void* cv);

void Zeta_CircularVector_PopR(void* cv);

void* Zeta_CircularVector_Erase(void* cv, void* pos_cursor);

void Zeta_CircularVector_EraseAll(void* cv, void* callback_context,
                                  void (*Callback)(void* context, void* ele));

// void Zeta_CircularVector_Check(void* cv, Zeta_DebugTreeMap* dst_tm);

void Zeta_CircularVector_DeploySeqContainer(void* cv,
                                            Zeta_SeqContainer* seq_cntr);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_CircularVector_Cursor_Check(void* sv, void const* cursor);

bool_t Zeta_CircularVector_Cursor_IsEqual(void* cv, void const* cursor_a,
                                          void const* cursor_b);

int Zeta_CircularVector_Cursor_Compare(void* cv, void const* cursor_a,
                                       void const* cursor_b);

size_t Zeta_CircularVector_Cursor_Differ(void* cv, void const* cursor_a,
                                         void const* cursor_b);

void* Zeta_CircularVector_Cursor_Refer(void* cv, void const* cursor);

void Zeta_CircularVector_Cursor_StepL(void* cv, void* cursor);

void Zeta_CircularVector_Cursor_StepR(void* cv, void* cursor);

void Zeta_CircularVector_Cursor_AdvanceL(void* cv, void* cursor, size_t step);

void Zeta_CircularVector_Cursor_AdvanceR(void* cv, void* cursor, size_t step);

void Zeta_CircularVector_Cursor_DeployCursorOperator(
    void* cv, Zeta_CursorOperator* cursor_opr);

ZETA_ExternC_End;
