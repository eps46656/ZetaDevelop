#pragma once

#include "Allocator.h"
#include "Cursor.h"
#include "DebugTreeMap.h"
#include "OrdCntRBTreeNode.h"
#include "SeqContainer.h"

ZETA_extern_c_beg;

typedef struct Zeta_SegVector_Node Zeta_SegVector_Node;

struct Zeta_SegVector_Node {
    Zeta_OrdCntRBTreeNode n;
    void* seg;
    unsigned short offset;
    unsigned short size;
};

typedef struct Zeta_SegVector_Cursor Zeta_SegVector_Cursor;

struct Zeta_SegVector_Cursor {
    size_t idx;
    Zeta_OrdCntRBTreeNode* n;
    size_t seg_idx;
    void* ele;
};

typedef struct Zeta_SegVector Zeta_SegVector;

struct Zeta_SegVector {
    /** The width(bytes) of element occupying. */
    size_t width;

    /** The max number of elements in one seg. */
    size_t seg_capacity;

    Zeta_Allocator* node_allocator;
    Zeta_Allocator* seg_allocator;

    Zeta_OrdCntRBTreeNode lb;
    Zeta_OrdCntRBTreeNode rb;

    Zeta_OrdCntRBTreeNode* root;
};

void Zeta_SegVector_Init(void* sv);

size_t Zeta_SegVector_GetWidth(void* sv);

size_t Zeta_SegVector_GetSize(void* sv);

void Zeta_SegVector_GetLB(void* sv, void* dst_cursor);

void Zeta_SegVector_GetRB(void* sv, void* dst_cursor);

void* Zeta_SegVector_PeekL(void* sv, void* dst_cursor);

void* Zeta_SegVector_PeekR(void* sv, void* dst_cursor);

void* Zeta_SegVector_Access(void* sv, void* dst_cursor, size_t idx);

void* Zeta_SegVector_FindFirst(void* sv, void* dst_cursor, void* context,
                               bool_t (*Compare)(void* context, void* ele));

void* Zeta_SegVector_PushL(void* sv, void* dst_cursor);

void* Zeta_SegVector_PushR(void* sv, void* dst_cursor);

void* Zeta_SegVector_Insert(void* sv, void* pos_cursor);

void Zeta_SegVector_PopL(void* sv);

void Zeta_SegVector_PopR(void* sv);

void* Zeta_SegVector_Erase(void* sv, void* pos_cursor);

void Zeta_SegVector_EraseAll(void* sv, void* callabck_context,
                             void (*Callback)(void* context, void* ele));

void Zeta_SegVector_Check(void* sv, Zeta_DebugTreeMap* dst_node_tm,
                          Zeta_DebugTreeMap* dst_seg_tm);

void Zeta_SegVector_DeploySeqContainer(void* sv, Zeta_SeqContainer* seq_cntr);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_SegVector_Cursor_Check(void* sv, void const* cursor);

bool_t Zeta_SegVector_Cursor_IsEqual(void* sv, void const* cursor_a,
                                     void const* cursor_b);

int Zeta_SegVector_Cursor_Compare(void* sv, void const* cursor_a,
                                  void const* cursor_b);

size_t Zeta_SegVector_Cursor_Differ(void* sv, void const* cursor_a,
                                    void const* cursor_b);

void Zeta_SegVector_Cursor_StepL(void* sv, void* cursor);

void Zeta_SegVector_Cursor_StepR(void* sv, void* cursor);

void Zeta_SegVector_Cursor_AdvanceL(void* sv, void* cursor, size_t step);

void Zeta_SegVector_Cursor_AdvanceR(void* sv, void* cursor, size_t step);

void Zeta_SegVector_Cursor_DeployCursorOperator(
    void* sv, Zeta_CursorOperator* cursor_opr);

ZETA_extern_c_end;
