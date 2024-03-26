#pragma once

#include "Allocator.h"
#include "Cursor.h"
#include "DebugTreeMap.h"
#include "OrdCntRBTreeNode.h"
#include "Vector.h"

ZETA_extern_c_beg;

typedef struct Zeta_SegVector_Node Zeta_SegVector_Node;

struct Zeta_SegVector_Node {
    char padding[8];
    Zeta_OrdCntRBTreeNode n;
    void* seg;
    size_t offset;
};

typedef struct Zeta_SegVector_Iter Zeta_SegVector_Iter;

struct Zeta_SegVector_Iter {
    Zeta_OrdCntRBTreeNode* n;
    size_t seg_idx;
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

void* Zeta_SegVector_PeekL(void* sv);

void* Zeta_SegVector_PeekR(void* sv);

Zeta_Cursor Zeta_SegVector_Access(void* sv, size_t idx);

Zeta_Cursor Zeta_SegVector_FindFirst(void* sv_, size_t* dst_idx, void** dst_ele,
                                     void* context,
                                     bool_t (*Compare)(void* context,
                                                       void* ele));

void* Zeta_SegVector_PushL(void* sv);

void* Zeta_SegVector_PushR(void* sv);

Zeta_Cursor Zeta_SegVector_Insert(void* sv, Zeta_Cursor pos);

void Zeta_SegVector_PopL(void* sv);

void Zeta_SegVector_PopR(void* sv);

void* Zeta_SegVector_Erase(void* sv, void* cursor);

void Zeta_SegVector_Check(void* sv_, Zeta_DebugTreeMap* dst_node_tm,
                          Zeta_DebugTreeMap* dst_seg_tm);

void Zeta_SegVector_ToVector(void* sv, Zeta_Vector* dst);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

bool_t Zeta_SegVector_Cursor_IsEqual(void* sv, Zeta_Cursor const* cursor_a,
                                     Zeta_Cursor const* cursor_b);

int Zeta_SegVector_Cursor_Compare(void* sv, Zeta_Cursor const* cursor_a,
                                  Zeta_Cursor const* cursor_b);

size_t Zeta_SegVector_Cursor_Diff(void* sv, Zeta_Cursor const* cursor_a,
                                  Zeta_Cursor const* cursor_b);

void Zeta_SegVector_Cursor_StepL(void* sv, Zeta_Cursor* cursor);

void Zeta_SegVector_Cursor_StepR(void* sv, Zeta_Cursor* cursor);

void Zeta_SegVector_Cursor_AdvanceL(void* sv, Zeta_Cursor* cursor, size_t step);

void Zeta_SegVector_Cursor_AdvanceR(void* sv, Zeta_Cursor* cursor, size_t step);

ZETA_extern_c_end;
