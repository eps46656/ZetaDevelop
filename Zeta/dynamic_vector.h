#pragma once

#include "allocator.h"
#include "mem_check_utils.h"
#include "seq_cntr.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_DynamicVector);
ZETA_DeclareStruct(Zeta_DynamicVector_Cursor);

struct Zeta_DynamicVector {
    size_t width;

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

    Zeta_Allocator data_allocator;

    Zeta_Allocator seg_allocator;
};

struct Zeta_DynamicVector_Cursor {
    void const* dv;
    size_t idx;
    void* ref;
};

void Zeta_DynamicVector_Init(void* dv);

void Zeta_DynamicVector_Deinit(void* dv);

size_t Zeta_DynamicVector_GetWidth(void const* dv);

size_t Zeta_DynamicVector_GetSize(void const* dv);

size_t Zeta_DynamicVector_GetCapacity(void const* dv);

void Zeta_DynamicVector_GetLBCursor(void const* dv, void* dst_cursor);

void Zeta_DynamicVector_GetRBCursor(void const* dv, void* dst_cursor);

void* Zeta_DynamicVector_PeekL(void* dv, void* dst_cursor, void* dst_elem);

void* Zeta_DynamicVector_PeekR(void* dv, void* dst_cursor, void* dst_elem);

void* Zeta_DynamicVector_Refer(void* dv, void const* pos_cursor);

void* Zeta_DynamicVector_Access(void* dv, size_t idx, void* dst_cursor,
                                void* dst_elem);

void Zeta_DynamicVector_Read(void const* dv, void const* pos_cursor, size_t cnt,
                             void* dst, void* dst_cursor);

void Zeta_DynamicVector_Write(void* dv, void* pos_cursor, size_t cnt,
                              void const* src, void* dst_cursor);

void* Zeta_DynamicVector_PushL(void* dv, size_t cnt, void* dst_cursor);

void* Zeta_DynamicVector_PushR(void* dv, size_t cnt, void* dst_cursor);

void Zeta_DynamicVector_PopL(void* dv, size_t cnt);

void Zeta_DynamicVector_PopR(void* dv, size_t cnt);

void Zeta_DynamicVector_EraseAll(void* dv);

void Zeta_DynamicVector_Check(void const* dv);

void Zeta_DynamicVector_Sanitize(void* dv, Zeta_MemRecorder* dst_data,
                                 Zeta_MemRecorder* dst_seg);

bool_t Zeta_DynamicVector_Cursor_AreEqual(void const* dv, void const* cursor_a,
                                          void const* cursor_b);

int Zeta_DynamicVector_Cursor_Compare(void const* dv, void const* cursor_a,
                                      void const* cursor_b);

size_t Zeta_DynamicVector_Cursor_GetDist(void const* dv, void const* cursor_a,
                                         void const* cursor_b);

size_t Zeta_DynamicVector_Cursor_GetIdx(void const* dv, void const* cursor);

void Zeta_DynamicVector_Cursor_StepL(void const* dv, void* cursor);

void Zeta_DynamicVector_Cursor_StepR(void const* dv, void* cursor);

void Zeta_DynamicVector_Cursor_AdvanceL(void const* dv, void* cursor,
                                        size_t step);

void Zeta_DynamicVector_Cursor_AdvanceR(void const* dv, void* cursor,
                                        size_t step);

void Zeta_DynamicVector_Cursor_Check(void const* dv, void const* cursor);

extern Zeta_SeqCntr_VTable const zeta_dynamic_vector_seq_cntr_vtable;

ZETA_ExternC_End;
