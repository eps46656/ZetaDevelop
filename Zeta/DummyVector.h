#pragma once

#include "SeqContainer.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DummyVector);

struct Zeta_DummyVector {
    size_t width;
    size_t stride;
};

size_t Zeta_DummyVector_GetWidth(void* dsc);

size_t Zeta_DummyVector_GetStride(void* dsc);

size_t Zeta_DummyVector_GetSize(void* dsc);

void Zeta_DummyVector_GetLBCursor(void* dsc, void* dst_cursor);

void Zeta_DummyVector_GetRBCursor(void* dsc, void* dst_cursor);

void* Zeta_DummyVector_PeekL(void* dsc, void* dst_cursor, void* dst_ele);

void* Zeta_DummyVector_PeekR(void* dsc, void* dst_cursor, void* dst_ele);

void* Zeta_DummyVector_Access(void* dsc, void* dst_cursor, void* dst_ele,
                              size_t idx);

void* Zeta_DummyVector_Refer(void* dsc, void* pos_cursor);

void Zeta_DummyVector_Read(void* dsc, void* pos_cursor, size_t cnt, void* dst,
                           void* dst_cursor);

bool_t Zeta_DummyVector_Cursor_IsEqual(void* dsc, void const* cursor_a,
                                       void const* cursor_b);

int Zeta_DummyVector_Cursor_Compare(void* dsc, void const* cursor_a,
                                    void const* cursor_b);

size_t Zeta_DummyVector_Cursor_GetDist(void* dsc, void const* cursor_a,
                                       void const* cursor_b);

size_t Zeta_DummyVector_Cursor_GetIdx(void* dsc, void const* cursor);

void Zeta_DummyVector_Cursor_StepL(void* dsc, void* cursor);

void Zeta_DummyVector_Cursor_StepR(void* dsc, void* cursor);

void Zeta_DummyVector_Cursor_AdvanceL(void* dsc, void* cursor, size_t step);

void Zeta_DummyVector_Cursor_AdvanceR(void* dsc, void* cursor, size_t step);

void Zeta_DummyVector_DeploySeqContainer(void* dsc,
                                         Zeta_SeqContainer* seq_cntr);

ZETA_ExternC_End;
