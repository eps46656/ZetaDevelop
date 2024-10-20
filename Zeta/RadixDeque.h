#pragma once

#include "Allocator.h"
#include "MemCheck.h"
#include "SeqContainer.h"

ZETA_ExternC_Beg;

#define ZETA_RadixDeque_min_branch_num (2)
#define ZETA_RadixDeque_max_branch_num (ZETA_RangeMaxOf(unsigned char) / 2)

#define ZETA_RadixDeque_max_seg_capacity ZETA_RangeMaxOf(unsigned char)

#define ZETA_RadixDeque_max_order (12)

ZETA_DeclareStruct(Zeta_RadixDeque);
ZETA_DeclareStruct(Zeta_RadixDeque_Cursor);

struct Zeta_RadixDeque {
    size_t width;
    size_t stride;

    unsigned char seg_capacity;

    unsigned char branch_num;

    unsigned char order;

    unsigned char offset;

    size_t size;

    void** roots_lw[ZETA_RadixDeque_max_order + 1];
    void** roots_rw[ZETA_RadixDeque_max_order + 1];

    void** redundant_roots_lw[ZETA_RadixDeque_max_order + 1];
    void** redundant_roots_rw[ZETA_RadixDeque_max_order + 1];

    unsigned char roots_cnt_lw[ZETA_RadixDeque_max_order + 1];
    unsigned char roots_cnt_rw[ZETA_RadixDeque_max_order + 1];

    Zeta_Allocator* node_allocator;
    Zeta_Allocator* seg_allocator;
};

struct Zeta_RadixDeque_Cursor {
    void* rd;
    size_t idx;
    void* seg;
    size_t seg_idx;
    void* ref;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_RadixDeque_Init(void* rd);

void Zeta_RadixDeque_Deinit(void* rd);

size_t Zeta_RadixDeque_GetWidth(void* rd);

size_t Zeta_RadixDeque_GetStride(void* rd);

size_t Zeta_RadixDeque_GetSize(void* rd);

size_t Zeta_RadixDeque_GetCapacity(void* rd);

void Zeta_RadixDeque_GetLBCursor(void* rd, void* dst_cursor);

void Zeta_RadixDeque_GetRBCursor(void* rd, void* dst_cursor);

void* Zeta_RadixDeque_PeekL(void* rd, void* dst_cursor, void* dst_elem);

void* Zeta_RadixDeque_PeekR(void* rd, void* dst_cursor, void* dst_elem);

void* Zeta_RadixDeque_Refer(void* rd, void const* pos_cursor);

void* Zeta_RadixDeque_Access(void* rd, size_t idx, void* dst_cursor,
                             void* dst_elem);

void Zeta_RadixDeque_Read(void* rd, void const* pos_cursor, size_t cnt,
                          void* dst, void* dst_cursor);

void Zeta_RadixDeque_Write(void* rd, void* pos_cursor, size_t cnt,
                           void const* src, void* dst_cursor);

void* Zeta_RadixDeque_PushL(void* rd, size_t cnt, void* dst_cursor);

void* Zeta_RadixDeque_PushR(void* rd, size_t cnt, void* dst_cursor);

void Zeta_RadixDeque_PopL(void* rd, size_t cnt);

void Zeta_RadixDeque_PopR(void* rd, size_t cnt);

void Zeta_RadixDeque_EraseAll(void* rd);

void Zeta_RadixDeque_Check(void* rd);

void Zeta_RadixDeque_Sanitize(void* rd, Zeta_MemRecorder* dst_node,
                              Zeta_MemRecorder* dst_seg);

bool_t Zeta_RadixDeque_Cursor_AreEqual(void* rd, void const* cursor_a,
                                       void const* cursor_b);

int Zeta_RadixDeque_Cursor_Compare(void* rd, void const* cursor_a,
                                   void const* cursor_b);

size_t Zeta_RadixDeque_Cursor_GetDist(void* rd, void const* cursor_a,
                                      void const* cursor_b);

size_t Zeta_RadixDeque_Cursor_GetIdx(void* rd, void const* cursor);

void Zeta_RadixDeque_Cursor_StepL(void* rd, void* cursor);

void Zeta_RadixDeque_Cursor_StepR(void* rd, void* cursor);

void Zeta_RadixDeque_Cursor_AdvanceL(void* rd, void* cursor, size_t step);

void Zeta_RadixDeque_Cursor_AdvanceR(void* rd, void* cursor, size_t step);

void Zeta_RadixDeque_Cursor_Check(void* rd, void const* cursor);

void Zeta_RadixDeque_DeploySeqContainer(void* rd, Zeta_SeqContainer* seq_cntr);

ZETA_ExternC_End;

/*

seq_capacity * (

B * (B + R - 1) * 2                 (B + R - 1) * 2
B**2 * (B + R - 1) * 2              (B + R - 1) * 2
B**3 * (B + R - 1) * 2              (B + R - 1) * 2

)

0, 1, 2, 3, ~ , max_order - 1, max_order, max_order - 1, ~ , 3, 2, 1, 0

0
0, 1, 0
0, 1, 2, 1, 0
0, 1, 2, 3, 2, 1, 0

tree_roots:
    circular vector, void*
        tree_roots_offset
        tree_roots_size
        tree_roots_capacity
        tree_roots_data

    a circular vector which store the root of trees.

    tree_roots[i] = the root of ith tree

tree_

tree_orders_lw

*/
