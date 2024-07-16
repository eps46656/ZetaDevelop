#pragma once

#include "Allocator.h"
#include "Cursor.h"
#include "DebugHashMap.h"
#include "OrdCnt3RBTreeNode.h"
#include "SeqContainer.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_StageVector);
ZETA_DeclareStruct(Zeta_StageVector_Seg);
ZETA_DeclareStruct(Zeta_StageVector_Cursor);
ZETA_DeclareStruct(Zeta_StageVector_Stats);

#define ZETA_StageVector_ref_color (0)
#define ZETA_StageVector_dat_color (1)

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct Zeta_StageVector {
    /** The maximum number of elements in a Stagement. */
    size_t seg_capacity;

    Zeta_SeqContainer* origin;

    Zeta_OrdCnt3RBTreeNode* root;

    Zeta_OrdCnt3RBTreeNode* lb;
    Zeta_OrdCnt3RBTreeNode* rb;

    Zeta_Allocator* seg_allocator;
    Zeta_Allocator* data_allocator;
};

struct Zeta_StageVector_Seg {
    Zeta_OrdCnt3RBTreeNode n;

    union {
        struct {
            size_t beg;
            size_t size;
        } ref;

        struct {
            void* data;
            unsigned short offset;
            unsigned short size;
        } dat;
    };
};

struct Zeta_StageVector_Cursor {
    void* sv;
    size_t idx;
    void* n;
    size_t seg_idx;
    void* ele;
};

struct Zeta_StageVector_Stats {
    size_t ref_seg_cnt;
    size_t dat_seg_cnt;
    size_t ref_size;
    size_t dat_size;
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

void Zeta_StageVector_EraseAll(void* sv);

void Zeta_StageVector_Reset(void* sv);

void Zeta_StageVector_Check(void* sv, Zeta_DebugHashMap* dst_seg_hm,
                            Zeta_DebugHashMap* dst_data_hm);

void Zeta_StageVector_PrintState(void* sv);

Zeta_StageVector_Stats Zeta_StageVector_GetStats(void* sv);

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

ZETA_ExternC_End;

/*

i = 0 ~ N-1

x[-1] = seg_capacity
x[i] = [1, seg_capacity-1]
x[N] = seg_capacity

for i in [0, N-1]
    seg_capacity < x[i-1] + x[i] or
    seg_capacity < x[i] + x[i+1]

minimize sum(x[i] for i in [0, N-1]) / seg_capacity * N

*/
