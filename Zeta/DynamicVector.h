#pragma once

#include "Allocator.h"
#include "DebugHashMap.h"
#include "SeqContainer.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DynamicVector);
ZETA_DeclareStruct(Zeta_DynamicVector_Cursor);

struct Zeta_DynamicVector {
    size_t width;
    size_t stride;

    size_t seg_capacity;

    size_t offset;

    size_t size;

    size_t size_a;
    size_t size_b;
    size_t size_c;

    void* cur_data;
    size_t cur_offset;
    size_t cur_capacity;

    void* nxt_data;
    size_t nxt_offset;
    size_t nxt_capacity;

    Zeta_Allocator* data_allocator;
    Zeta_Allocator* seg_allocator;
};

struct Zeta_DynamicVector_Cursor {
    void* dv;
    size_t idx;
    void* ele;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_DynamicVector_Init(void* dv);

void Zeta_DynamicVector_Deinit(void* dv);

size_t Zeta_DynamicVector_GetWidth(void* dv);

size_t Zeta_DynamicVector_GetStride(void* dv);

size_t Zeta_DynamicVector_GetSize(void* dv);

size_t Zeta_DynamicVector_GetCapacity(void* dv);

void Zeta_DynamicVector_GetLBCursor(void* dv, void* dst_cursor);

void Zeta_DynamicVector_GetRBCursor(void* dv, void* dst_cursor);

void* Zeta_DynamicVector_PeekL(void* dv, void* dst_cursor, void* dst_ele);

void* Zeta_DynamicVector_PeekR(void* dv, void* dst_cursor, void* dst_ele);

void* Zeta_DynamicVector_Refer(void* dv, void const* pos_cursor);

void* Zeta_DynamicVector_Access(void* dv, void* dst_cursor, void* dst_ele,
                                size_t idx);

void Zeta_DynamicVector_Read(void* dv, void const* pos_cursor, size_t cnt,
                             void* dst, void* dst_cursor);

void Zeta_DynamicVector_Write(void* dv, void* pos_cursor, size_t cnt,
                              void const* src, void* dst_cursor);

void* Zeta_DynamicVector_PushL(void* dv, void* dst_cursor, size_t cnt);

void* Zeta_DynamicVector_PushR(void* dv, void* dst_cursor, size_t cnt);

void Zeta_DynamicVector_PopL(void* dv, size_t cnt);

void Zeta_DynamicVector_PopR(void* dv, size_t cnt);

void Zeta_DynamicVector_EraseAll(void* dv);

void Zeta_DynamicVector_Check(void* dv, Zeta_DebugHashMap* dst_data_hm,
                              Zeta_DebugHashMap* dst_seg_hm);

bool_t Zeta_DynamicVector_Cursor_IsEqual(void* dv, void const* cursor_a,
                                         void const* cursor_b);

int Zeta_DynamicVector_Cursor_Compare(void* dv, void const* cursor_a,
                                      void const* cursor_b);

size_t Zeta_DynamicVector_Cursor_GetDist(void* dv, void const* cursor_a,
                                         void const* cursor_b);

size_t Zeta_DynamicVector_Cursor_GetIdx(void* dv, void const* cursor);

void Zeta_DynamicVector_Cursor_StepL(void* dv, void* cursor);

void Zeta_DynamicVector_Cursor_StepR(void* dv, void* cursor);

void Zeta_DynamicVector_Cursor_AdvanceL(void* dv, void* cursor, size_t step);

void Zeta_DynamicVector_Cursor_AdvanceR(void* dv, void* cursor, size_t step);

void Zeta_DynamicVector_Cursor_Check(void* dv, void const* cursor);

void Zeta_DynamicVector_DeploySeqContainer(void* dv,
                                           Zeta_SeqContainer* seq_cntr);

ZETA_ExternC_End;
