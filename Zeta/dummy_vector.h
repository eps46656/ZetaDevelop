#pragma once

#include "seq_cntr.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DummyVector);

struct Zeta_DummyVector {
    size_t width;
};

size_t Zeta_DummyVector_GetWidth(void* dv);

size_t Zeta_DummyVector_GetSize(void* dv);

size_t Zeta_DummyVector_GetCapacity(void* dv);

void Zeta_DummyVector_GetLBCursor(void* dv, void* dst_cursor);

void Zeta_DummyVector_GetRBCursor(void* dv, void* dst_cursor);

void* Zeta_DummyVector_PeekL(void* dv, void* dst_cursor, void* dst_elem);

void* Zeta_DummyVector_PeekR(void* dv, void* dst_cursor, void* dst_elem);

void* Zeta_DummyVector_Access(void* dv, size_t idx, void* dst_cursor,
                              void* dst_elem);

void* Zeta_DummyVector_Refer(void* dv, void const* pos_cursor);

void Zeta_DummyVector_Read(void* dv, void const* pos_cursor, size_t cnt,
                           void* dst, void* dst_cursor);

void Zeta_DummyVector_Write(void* dv, void const* pos_cursor, size_t cnt,
                            void const* src, void* dst_cursor);

void Zeta_DummyVector_Check(void* dv);

bool_t Zeta_DummyVector_Cursor_AreEqual(void* dv, void const* cursor_a,
                                        void const* cursor_b);

int Zeta_DummyVector_Cursor_Compare(void* dv, void const* cursor_a,
                                    void const* cursor_b);

size_t Zeta_DummyVector_Cursor_GetDist(void* dv, void const* cursor_a,
                                       void const* cursor_b);

size_t Zeta_DummyVector_Cursor_GetIdx(void* dv, void const* cursor);

void Zeta_DummyVector_Cursor_StepL(void* dv, void* cursor);

void Zeta_DummyVector_Cursor_StepR(void* dv, void* cursor);

void Zeta_DummyVector_Cursor_AdvanceL(void* dv, void* cursor, size_t step);

void Zeta_DummyVector_Cursor_AdvanceR(void* dv, void* cursor, size_t step);

void Zeta_DummyVector_Cursor_Check(void* dv, void const* cursor);

void Zeta_DummyVector_DeploySeqCntr(void* dv, Zeta_SeqCntr* seq_cntr);

ZETA_ExternC_End;
