#pragma once

#include "Allocator.h"
#include "Cursor.h"
#include "OrdCnt3RBTreeNode.h"
#include "SeqContainer.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_StageVector);
ZETA_DeclareStruct(Zeta_StageVector_Node);
ZETA_DeclareStruct(Zeta_StageVector_Cursor);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct Zeta_StageVector {
    Zeta_SeqContainer* origin;

    /** The maximum number of elements in a Stagement. */
    size_t seg_capacity;

    Zeta_OrdCnt3RBTreeNode* root;

    Zeta_OrdCnt3RBTreeNode* lb;
    Zeta_OrdCnt3RBTreeNode* rb;

    Zeta_Allocator* node_allocator;
    Zeta_Allocator* seg_allocator;
};

struct Zeta_StageVector_Node {
    Zeta_OrdCnt3RBTreeNode n;

    union {
        struct {
            size_t beg;
        };

        struct {
            void* seg;
            unsigned short offset;
        };
    };

    size_t size;
};

struct Zeta_StageVector_Cursor {
    void* sv;
    size_t idx;
    void* n;
    size_t seg_idx;
    void* ele;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_StageVector_Init(void* sv);

void Zeta_StageVector_Deinit(void* sv);

size_t Zeta_StageVector_GetWidth(void* sv);

size_t Zeta_StageVector_GetStride(void* sv);

size_t Zeta_StageVector_GetSize(void* sv);

void Zeta_StageVector_GetLBCursor(void* sv, void* dst_cursor);

void Zeta_StageVector_GetRBCursor(void* sv, void* dst_cursor);

void* Zeta_StageVector_FindFirst(void* sv, void* dst_cursor, void* dst_ele,
                                 void* context,
                                 bool_t (*Compare)(void* context, void* ele));

void* Zeta_StageVector_PeekL(void* sv, void* dst_cursor, void* dst_ele);

void* Zeta_StageVector_PeekR(void* sv, void* dst_cursor, void* dst_ele);

void* Zeta_StageVector_Access(void* sv, void* dst_cursor, void* dst_ele,
                              size_t idx);

void* Zeta_StageVector_Refer(void* sv, void* pos_cursor);

void Zeta_StageVector_Read(void* sv, void* pos_cursor, size_t cnt, void* dst,
                           void* dst_cursor);

void Zeta_StageVector_Write(void* sv, void* pos_cursor, size_t cnt,
                            void const* src, void* dst_cursor);

void* Zeta_StageVector_PushL(void* sv, void* dst_cursor);

void* Zeta_StageVector_PushR(void* sv, void* dst_cursor);

void* Zeta_StageVector_Insert(void* sv, void* pos_cursor, size_t cnt);

void Zeta_StageVector_PopL(void* sv);

void Zeta_StageVector_PopR(void* sv);

void Zeta_StageVector_Erase(void* sv, void* pos_cursor, size_t cnt);

void Zeta_StageVector_EraseAll(void* sv, void* context,
                               void (*Callback)(void* context, void* ele));

void Zeta_StageVector_Check(void* sv);

bool_t Zeta_StageVector_Cursor_IsEqual(void* sv, void const* cursor_a,
                                       void const* cursor_b);

int Zeta_StageVector_Cursor_Compare(void* sv, void const* cursor_a,
                                    void const* cursor_b);

size_t Zeta_StageVector_Cursor_GetDist(void* sv, void const* cursor_a,
                                       void const* cursor_b);

size_t Zeta_StageVector_Cursor_GetIdx(void* sv, void const* cursor);

void Zeta_StageVector_Cursor_StepL(void* sv, void* cursor);

void Zeta_StageVector_Cursor_StepR(void* sv, void* cursor);

void Zeta_StageVector_Cursor_AdvanceL(void* sv, void* cursor, size_t step);

void Zeta_StageVector_Cursor_AdvanceR(void* sv, void* cursor, size_t step);

void Zeta_StageVector_Cursor_Check(void* sv, void const* cursor);

void Zeta_StageVector_DeploySeqContainer(void* sv, Zeta_SeqContainer* seq_cntr);

void Zeta_StageVector_DeploySeqContainer(void* sv, Zeta_SeqContainer* seq_cntr);

ZETA_ExternC_End;
