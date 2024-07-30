#pragma once

#include "Allocator.h"
#include "DebugHashMap.h"
#include "SeqContainer.h"

ZETA_ExternC_Beg;

#define ZETA_RadixVector_min_branch_num (2)
#define ZETA_RadixVector_max_branch_num (ZETA_RangeMaxOf(unsigned char) / 2)

#define ZETA_RadixVector_max_seg_capacity ZETA_RangeMaxOf(unsigned char)

#define ZETA_RadixVector_max_order (12)

ZETA_DeclareStruct(Zeta_RadixVector);
ZETA_DeclareStruct(Zeta_RadixVector_Cursor);

struct Zeta_RadixVector {
    size_t width;
    size_t stride;

    unsigned char seg_capacity;

    unsigned char branch_num;

    unsigned char order;

    size_t size;

    void** roots[ZETA_RadixVector_max_order + 1];
    void** redundant_roots[ZETA_RadixVector_max_order + 1];

    unsigned char roots_cnt[ZETA_RadixVector_max_order + 1];

    Zeta_Allocator* node_allocator;
    Zeta_Allocator* seg_allocator;
};

struct Zeta_RadixVector_Cursor {
    void* rv;
    size_t idx;
    void* seg;
    size_t seg_idx;
    void* ele;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_RadixVector_Init(void* rv);

void Zeta_RadixVector_Deinit(void* rv);

size_t Zeta_RadixVector_GetWidth(void* rv);

size_t Zeta_RadixVector_GetStride(void* rv);

size_t Zeta_RadixVector_GetSize(void* rv);

size_t Zeta_RadixVector_GetCapacity(void* rv);

void Zeta_RadixVector_GetLBCursor(void* rv, void* dst_cursor);

void Zeta_RadixVector_GetRBCursor(void* rv, void* dst_cursor);

void* Zeta_RadixVector_PeekL(void* rv, void* dst_cursor, void* dst_ele);

void* Zeta_RadixVector_PeekR(void* rv, void* dst_cursor, void* dst_ele);

void* Zeta_RadixVector_Access(void* rv, void* dst_cursor, void* dst_ele,
                              size_t idx);

void Zeta_RadixVector_Read(void* rv, void const* pos_cursor, size_t cnt,
                           void* dst, void* dst_cursor);

void Zeta_RadixVector_Write(void* rv, void* pos_cursor, size_t cnt,
                            void const* src, void* dst_cursor);

void* Zeta_RadixVector_PushR(void* rv, void* dst_cursor, size_t cnt);

void Zeta_RadixVector_PopR(void* rv, size_t cnt);

void Zeta_RadixVector_EraseAll(void* rv);

void Zeta_RadixVector_Check(void* rv, Zeta_DebugHashMap* dst_node_hm,
                            Zeta_DebugHashMap* dst_seg_hm);

bool_t Zeta_RadixVector_Cursor_IsEqual(void* rv, void const* cursor_a,
                                       void const* cursor_b);

int Zeta_RadixVector_Cursor_Compare(void* rv, void const* cursor_a,
                                    void const* cursor_b);

size_t Zeta_RadixVector_Cursor_GetDist(void* rv, void const* cursor_a,
                                       void const* cursor_b);

size_t Zeta_RadixVector_Cursor_GetIdx(void* rv, void const* cursor);

void Zeta_RadixVector_Cursor_StepL(void* rv, void* cursor);

void Zeta_RadixVector_Cursor_StepR(void* rv, void* cursor);

void Zeta_RadixVector_Cursor_AdvanceL(void* rv, void* cursor, size_t step);

void Zeta_RadixVector_Cursor_AdvanceR(void* rv, void* cursor, size_t step);

void Zeta_RadixVector_Cursor_Check(void* rv, void const* cursor);

void Zeta_RadixVector_DeploySeqContainer(void* rv, Zeta_SeqContainer* seq_cntr);

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
