#pragma once

#include "Allocator.h"
#include "DebugHashTable.h"
#include "OrdCntRBTreeNode.h"
#include "SeqContainer.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_SegVector);
ZETA_DeclareStruct(Zeta_SegVector_Cursor);
ZETA_DeclareStruct(Zeta_SegVector_Node);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct Zeta_SegVector {
    /** The number of bytes an element occupying. */
    size_t width;

    /** The number of bytes between the begins of two adjacent elements. */
    size_t stride;

    /** The maximum number of elements in a segment. */
    size_t seg_capacity;

    Zeta_OrdCntRBTreeNode* lb;
    Zeta_OrdCntRBTreeNode* rb;

    void* root;

    Zeta_Allocator* node_allocator;
    Zeta_Allocator* seg_allocator;
};

struct Zeta_SegVector_Cursor {
    Zeta_SegVector* sv;
    size_t idx;
    void* n;
    size_t seg_idx;
    void* ele;
};

struct Zeta_SegVector_Node {
    Zeta_OrdCntRBTreeNode n;
    void* seg;
    unsigned short offset;
    unsigned short size;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_SegVector_Init(void* sv);

void Zeta_SegVector_Deinit(void* sv);

size_t Zeta_SegVector_GetWidth(void* sv);

size_t Zeta_SegVector_GetStride(void* sv);

size_t Zeta_SegVector_GetSize(void* sv);

void Zeta_SegVector_GetLBCursor(void* sv, void* dst_cursor);

void Zeta_SegVector_GetRBCursor(void* sv, void* dst_cursor);

void* Zeta_SegVector_FindFirst(void* sv, void* dst_cursor, void* dst_elem,
                               void* context,
                               bool_t (*Compare)(void* context, void* ele));

void* Zeta_SegVector_PeekL(void* sv, void* dst_cursor, void* dst_elem);

void* Zeta_SegVector_PeekR(void* sv, void* dst_cursor, void* dst_elem);

void* Zeta_SegVector_Access(void* sv, void* dst_cursor, void* dst_elem,
                            size_t idx);

void* Zeta_SegVector_Refer(void* sv, void* pos_cursor);

void Zeta_SegVector_Read(void* sv, void* pos_cursor, size_t cnt, void* dst,
                         void* dst_cursor);

void Zeta_SegVector_Write(void* sv, void* pos_cursor, size_t cnt,
                          void const* src, void* dst_cursor);

void* Zeta_SegVector_PushL(void* sv, void* dst_cursor);

void* Zeta_SegVector_PushR(void* sv, void* dst_cursor);

void* Zeta_SegVector_Insert(void* sv, void* pos_cursor, size_t cnt);

void Zeta_SegVector_PopL(void* sv);

void Zeta_SegVector_PopR(void* sv);

void Zeta_SegVector_Erase(void* sv, void* pos_cursor, size_t cnt);

void Zeta_SegVector_EraseAll(void* sv);

void Zeta_SegVector_Check(void* sv, Zeta_DebugHashTable* dst_node_hm,
                          Zeta_DebugHashTable* dst_seg_hm);

bool_t Zeta_SegVector_Cursor_IsEqual(void* sv, void const* cursor_a,
                                     void const* cursor_b);

int Zeta_SegVector_Cursor_Compare(void* sv, void const* cursor_a,
                                  void const* cursor_b);

size_t Zeta_SegVector_Cursor_GetDist(void* sv, void const* cursor_a,
                                     void const* cursor_b);

size_t Zeta_SegVector_Cursor_GetIdx(void* sv, void const* cursor);

void Zeta_SegVector_Cursor_StepL(void* sv, void* cursor);

void Zeta_SegVector_Cursor_StepR(void* sv, void* cursor);

void Zeta_SegVector_Cursor_AdvanceL(void* sv, void* cursor, size_t step);

void Zeta_SegVector_Cursor_AdvanceR(void* sv, void* cursor, size_t step);

void Zeta_SegVector_Cursor_Check(void* sv, void const* cursor);

void Zeta_SegVector_DeploySeqContainer(void* sv, Zeta_SeqContainer* seq_cntr);

void Zeta_SegVector_DeploySeqContainer(void* sv, Zeta_SeqContainer* seq_cntr);

ZETA_ExternC_End;
