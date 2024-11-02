#pragma once

#include "seq_cntr.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DebugDeque);

struct Zeta_DebugDeque {
    void* deque;
    size_t width;
};

void Zeta_DebugDeque_Init(void* dd);

void Zeta_DebugDeque_Deinit(void* dd);

size_t Zeta_DebugDeque_GetWidth(void* dd);

size_t Zeta_DebugDeque_GetSize(void* dd);

size_t Zeta_DebugDeque_GetCapacity(void* dd);

void Zeta_DebugDeque_GetLBCursor(void* context, void* dst_cursor);

void Zeta_DebugDeque_GetRBCursor(void* context, void* dst_cursor);

void* Zeta_DebugDeque_PeekL(void* dd, void* dst_cursor, void* dst_elem);

void* Zeta_DebugDeque_PeekR(void* dd, void* dst_cursor, void* dst_elem);

void* Zeta_DebugDeque_Access(void* dd, size_t idx, void* dst_cursor,
                             void* dst_elem);

void* Zeta_DebugDeque_Refer(void* dd, void const* pos_cursor);

void Zeta_DebugDeque_Read(void* dd, void const* pos_cursor, size_t cnt,
                          void* dst, void* dst_cursor);

void Zeta_DebugDeque_Write(void* dd, void* pos_cursor, size_t cnt,
                           void const* src, void* dst_cursor);

void* Zeta_DebugDeque_PushL(void* dd, size_t cnt, void const* src,
                            void* dst_cursor);

void* Zeta_DebugDeque_PushR(void* dd, size_t cnt, void const* src,
                            void* dst_cursor);

void* Zeta_DebugDeque_Insert(void* dd, void* pos_cursor, size_t cnt,
                             void const* src);

void Zeta_DebugDeque_PopL(void* dd, size_t cnt);

void Zeta_DebugDeque_PopR(void* dd, size_t cnt);

void Zeta_DebugDeque_Erase(void* dd, void* pos_cursor, size_t cnt);

void Zeta_DebugDeque_EraseAll(void* dd);

void Zeta_DebugDeque_Check(void* dd);

bool_t Zeta_DebugDeque_Cursor_AreEqual(void* dd, void const* cursor_a,
                                       void const* cursor_b);

int Zeta_DebugDeque_Cursor_Compare(void* dd, void const* cursor_a,
                                   void const* cursor_b);

size_t Zeta_DebugDeque_Cursor_GetDist(void* dd, void const* cursor_a,
                                      void const* cursor_b);

size_t Zeta_DebugDeque_Cursor_GetIdx(void* dd, void const* cursor);

void Zeta_DebugDeque_Cursor_StepL(void* dd, void* cursor);

void Zeta_DebugDeque_Cursor_StepR(void* dd, void* cursor);

void Zeta_DebugDeque_Cursor_AdvanceL(void* dd, void* cursor, size_t step);

void Zeta_DebugDeque_Cursor_AdvanceR(void* dd, void* cursor, size_t step);

void Zeta_DebugDeque_Cursor_Check(void* dd, void const* cursor);

void Zeta_DebugDeque_DeploySeqCntr(void* dd, Zeta_SeqCntr* seq_cntr);

ZETA_ExternC_End;
