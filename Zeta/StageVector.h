#pragma once

#include "Allocator.h"
#include "MemCheck.h"
#include "OrdCnt3RBTreeNode.h"
#include "SeqContainer.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_StageVector);
ZETA_DeclareStruct(Zeta_StageVector_Seg);
ZETA_DeclareStruct(Zeta_StageVector_Cursor);
ZETA_DeclareStruct(Zeta_StageVector_Stats);

#define ZETA_StageVector_ref_color (0)
#define ZETA_StageVector_dat_color (1)

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
    void* ref;
};

struct Zeta_StageVector_Stats {
    size_t ref_seg_cnt;
    size_t dat_seg_cnt;
    size_t ref_size;
    size_t dat_size;
};

void Zeta_StageVector_Init(void* sv);

void Zeta_StageVector_Deinit(void* sv);

size_t Zeta_StageVector_GetWidth(void* sv);

size_t Zeta_StageVector_GetStride(void* sv);

size_t Zeta_StageVector_GetSize(void* sv);

size_t Zeta_StageVector_GetCapacity(void* sv);

void Zeta_StageVector_GetLBCursor(void* sv, void* dst_cursor);

void Zeta_StageVector_GetRBCursor(void* sv, void* dst_cursor);

void* Zeta_StageVector_FindFirst(void* sv, void* dst_cursor, void* dst_elem,
                                 void* context,
                                 bool_t (*Compare)(void* context, void* ele));

void* Zeta_StageVector_PeekL(void* sv, void* dst_cursor, void* dst_elem);

void* Zeta_StageVector_PeekR(void* sv, void* dst_cursor, void* dst_elem);

void* Zeta_StageVector_Access(void* sv, size_t idx, void* dst_cursor,
                              void* dst_elem);

void* Zeta_StageVector_Refer(void* sv, void const* pos_cursor);

void Zeta_StageVector_Read(void* sv, void const* pos_cursor, size_t cnt,
                           void* dst, void* dst_cursor);

void Zeta_StageVector_Write(void* sv, void* pos_cursor, size_t cnt,
                            void const* src, void* dst_cursor);

void* Zeta_StageVector_PushL(void* sv, size_t cnt, void* dst_cursor);

void* Zeta_StageVector_PushR(void* sv, size_t cnt, void* dst_cursor);

void* Zeta_StageVector_Insert(void* sv, void* pos_cursor, size_t cnt);

void Zeta_StageVector_PopL(void* sv, size_t cnt);

void Zeta_StageVector_PopR(void* sv, size_t cnt);

void Zeta_StageVector_Erase(void* sv, void* pos_cursor, size_t cnt);

void Zeta_StageVector_EraseAll(void* sv);

void Zeta_StageVector_Reset(void* sv);

void Zeta_StageVector_Check(void* sv);

void Zeta_StageVector_Sanitize(void* sv, Zeta_MemRecorder* dst_seg,
                               Zeta_MemRecorder* dst_data);

void Zeta_StageVector_PrintState(void* sv);

Zeta_StageVector_Stats Zeta_StageVector_GetStats(void* sv);

bool_t Zeta_StageVector_Cursor_AreEqual(void* sv, void const* cursor_a,
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
