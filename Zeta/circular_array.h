#pragma once

#include "seq_cntr.h"

ZETA_ExternC_Beg;

#define ZETA_CircularArray_max_capacity ZETA_GetMaxMod(size_t)

#define ZETA_CircularArray_vec_mem_copy_th (sizeof(void*) * 4 + 1)

ZETA_DeclareStruct(Zeta_CircularArray);
ZETA_DeclareStruct(Zeta_CircularArray_Cursor);

struct Zeta_CircularArray {
    void* data;
    size_t width;
    size_t stride;
    size_t offset;
    size_t size;
    size_t capacity;
};

struct Zeta_CircularArray_Cursor {
    Zeta_CircularArray* ca;
    size_t idx;
    void* ref;
};

void Zeta_CircularArray_Init(void* ca);

void Zeta_CircularArray_Deinit(void* ca);

size_t Zeta_CircularArray_GetWidth(void* ca);

size_t Zeta_CircularArray_GetStride(void* ca);

size_t Zeta_CircularArray_GetOffset(void* ca);

size_t Zeta_CircularArray_GetSize(void* ca);

size_t Zeta_CircularArray_GetCapacity(void* ca);

void Zeta_CircularArray_GetLBCursor(void* ca, void* dst_cursor);

void Zeta_CircularArray_GetRBCursor(void* ca, void* dst_cursor);

void* Zeta_CircularArray_PeekL(void* ca, void* dst_cursor, void* dst_elem);

void* Zeta_CircularArray_PeekR(void* ca, void* dst_cursor, void* dst_elem);

void* Zeta_CircularArray_Access(void* ca, size_t idx, void* dst_cursor,
                                void* dst_elem);

void* Zeta_CircularArray_Refer(void* ca, void const* pos_cursor);

void Zeta_CircularArray_Read(void* ca, void const* pos_cursor, size_t cnt,
                             void* dst, void* dst_cursor);

void Zeta_CircularArray_Write(void* ca, void* pos_cursor, size_t cnt,
                              void const* src, void* dst_cursor);

void* Zeta_CircularArray_PushL(void* ca, size_t cnt, void* dst_cursor);

void* Zeta_CircularArray_PushR(void* ca, size_t cnt, void* dst_cursor);

void* Zeta_CircularArray_Insert(void* ca, void* pos_cursor, size_t cnt);

void Zeta_CircularArray_PopL(void* ca, size_t cnt);

void Zeta_CircularArray_PopR(void* ca, size_t cnt);

void Zeta_CircularArray_Erase(void* ca, void* pos_cursor, size_t cnt);

void Zeta_CircularArray_EraseAll(void* ca);

size_t Zeta_CircularArray_GetLongestContPred(void* ca, size_t idx);

size_t Zeta_CircularArray_GetLongestContSucr(void* ca, size_t idx);

void Zeta_CircularArray_Assign(void* dst_ca, void* src_ca, size_t dst_beg,
                               size_t src_beg, size_t cnt);

void Zeta_CircularArray_AssignFromSeqCntr(void* ca, void* ca_cursor,
                                          Zeta_SeqCntr* seq_cntr,
                                          void* seq_cntr_cursor, size_t cnt);

void Zeta_CircularArray_Check(void* ca);

bool_t Zeta_CircularArray_Cursor_AreEqual(void* ca, void const* cursor_a,
                                          void const* cursor_b);

int Zeta_CircularArray_Cursor_Compare(void* ca, void const* cursor_a,
                                      void const* cursor_b);

size_t Zeta_CircularArray_Cursor_GetDist(void* ca, void const* cursor_a,
                                         void const* cursor_b);

size_t Zeta_CircularArray_Cursor_GetIdx(void* ca, void const* cursor);

void Zeta_CircularArray_Cursor_StepL(void* ca, void* cursor);

void Zeta_CircularArray_Cursor_StepR(void* ca, void* cursor);

void Zeta_CircularArray_Cursor_AdvanceL(void* ca, void* cursor, size_t step);

void Zeta_CircularArray_Cursor_AdvanceR(void* ca, void* cursor, size_t step);

void Zeta_CircularArray_Cursor_Check(void* ca, void const* cursor);

void Zeta_CircularArray_DeploySeqCntr(void* ca, Zeta_SeqCntr* seq_cntr);

ZETA_ExternC_End;
