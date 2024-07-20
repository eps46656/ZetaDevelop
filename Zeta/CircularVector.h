#pragma once

#include "Cursor.h"
#include "SeqContainer.h"

ZETA_ExternC_Beg;

#define ZETA_CircularVector_max_capacity ZETA_GetMaxMod(size_t)

#define ZETA_CircularVector_vec_mem_copy_th (sizeof(void*) * 4 + 1)

ZETA_DeclareStruct(Zeta_CircularVector);
ZETA_DeclareStruct(Zeta_CircularVector_Cursor);

struct Zeta_CircularVector {
    void* data;
    size_t width;
    size_t stride;
    size_t offset;
    size_t size;
    size_t capacity;
};

struct Zeta_CircularVector_Cursor {
    Zeta_CircularVector* cv;
    size_t idx;
    void* ele;
};

void Zeta_CircularVector_Init(void* cv);

size_t Zeta_CircularVector_GetWidth(void* cv);

size_t Zeta_CircularVector_GetStride(void* cv);

size_t Zeta_CircularVector_GetOffset(void* cv);

size_t Zeta_CircularVector_GetSize(void* cv);

size_t Zeta_CircularVector_GetCapacity(void* cv);

void Zeta_CircularVector_GetLBCursor(void* cv, void* dst_cursor);

void Zeta_CircularVector_GetRBCursor(void* cv, void* dst_cursor);

void* Zeta_CircularVector_PeekL(void* cv, void* dst_cursor, void* dst_ele);

void* Zeta_CircularVector_PeekR(void* cv, void* dst_cursor, void* dst_ele);

void* Zeta_CircularVector_Access(void* cv, void* dst_cursor, void* dst_ele,
                                 size_t idx);

void* Zeta_CircularVector_Refer(void* cv, void const* pos_cursor);

void Zeta_CircularVector_Read(void* cv, void const* pos_cursor, size_t cnt,
                              void* dst, void* dst_cursor);

void Zeta_CircularVector_Write(void* cv, void* pos_cursor, size_t cnt,
                               void const* src, void* dst_cursor);

void* Zeta_CircularVector_PushL(void* cv, void* dst_cursor, size_t cnt);

void* Zeta_CircularVector_PushR(void* cv, void* dst_cursor, size_t cnt);

void* Zeta_CircularVector_Insert(void* cv, void* pos_cursor, size_t cnt);

void Zeta_CircularVector_PopL(void* cv, size_t cnt);

void Zeta_CircularVector_PopR(void* cv, size_t cnt);

void Zeta_CircularVector_Erase(void* cv, void* pos_cursor, size_t cnt);

void Zeta_CircularVector_EraseAll(void* cv);

size_t Zeta_CircularVector_GetLongestContPred(void* cv, size_t idx);

size_t Zeta_CircularVector_GetLongestContSucr(void* cv, size_t idx);

void Zeta_CircularVector_Assign(void* dst_cv, void* src_cv, size_t dst_beg,
                                size_t src_beg, size_t cnt);

void Zeta_CircularVector_AssignFromSeqContainer(void* cv, void* cv_cursor,
                                                Zeta_SeqContainer* seq_cntr,
                                                void* seq_cntr_cursor,
                                                size_t cnt);

void Zeta_CircularVector_Check(void* cv);

bool_t Zeta_CircularVector_Cursor_IsEqual(void* cv, void const* cursor_a,
                                          void const* cursor_b);

int Zeta_CircularVector_Cursor_Compare(void* cv, void const* cursor_a,
                                       void const* cursor_b);

size_t Zeta_CircularVector_Cursor_GetDist(void* cv, void const* cursor_a,
                                          void const* cursor_b);

size_t Zeta_CircularVector_Cursor_GetIdx(void* cv, void const* cursor);

void Zeta_CircularVector_Cursor_StepL(void* cv, void* cursor);

void Zeta_CircularVector_Cursor_StepR(void* cv, void* cursor);

void Zeta_CircularVector_Cursor_AdvanceL(void* cv, void* cursor, size_t step);

void Zeta_CircularVector_Cursor_AdvanceR(void* cv, void* cursor, size_t step);

void Zeta_CircularVector_Cursor_Check(void* cv, void const* cursor);

void Zeta_CircularVector_DeploySeqContainer(void* cv,
                                            Zeta_SeqContainer* seq_cntr);

ZETA_ExternC_End;
