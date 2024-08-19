#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_SeqContainer);

struct Zeta_SeqContainer {
    void* context;

    size_t cursor_size;

    size_t (*GetWidth)(void* context);

    size_t (*GetStride)(void* context);

    size_t (*GetSize)(void* context);

    size_t (*GetCapacity)(void* context);

    void (*GetLBCursor)(void* context, void* dst_cursor);

    void (*GetRBCursor)(void* context, void* dst_cursor);

    void* (*PeekL)(void* context, void* dst_cursor, void* dst_elem);

    void* (*PeekR)(void* context, void* dst_cursor, void* dst_elem);

    void* (*Access)(void* context, void* dst_cursor, void* dst_elem,
                    size_t idx);

    void* (*Refer)(void* context, void const* pos_cursor);

    void (*Read)(void* context, void const* pos_cursor, size_t cnt, void* dst,
                 void* dst_cursor);

    void (*Write)(void* context, void* pos_cursor, size_t cnt, void const* src,
                  void* dst_cursor);

    void* (*PushL)(void* context, void* dst_cursor, size_t cnt);

    void* (*PushR)(void* context, void* dst_cursor, size_t cnt);

    void* (*Insert)(void* context, void* pos_cursor, size_t cnt);

    void (*PopL)(void* context, size_t cnt);

    void (*PopR)(void* context, size_t cnt);

    void (*Erase)(void* context, void* pos_cursor, size_t cnt);

    void (*EraseAll)(void* context);

    bool_t (*Cursor_IsEqual)(void* context, void const* cursor_a,
                             void const* cursor_b);

    int (*Cursor_Compare)(void* context, void const* cursor_a,
                          void const* cursor_b);

    size_t (*Cursor_GetDist)(void* context, void const* cursor_a,
                             void const* cursor_b);

    size_t (*Cursor_GetIdx)(void* context, void const* cursor);

    void (*Cursor_StepL)(void* context, void* cursor);

    void (*Cursor_StepR)(void* context, void* cursor);

    void (*Cursor_AdvanceL)(void* context, void* cursor, size_t step);

    void (*Cursor_AdvanceR)(void* context, void* cursor, size_t step);
};

void Zeta_SeqContainer_Init(Zeta_SeqContainer* seq_cntr);

#define ZETA_SeqContainer_AllocaCursor(seq_cntr)                     \
    __builtin_alloca_with_align(                                     \
        ((Zeta_SeqContainer*)ZETA_ToVoidPtr(seq_cntr))->cursor_size, \
        CHAR_BIT * alignof(max_align_t))

#define ZETA_SeqContainer_Call_(seq_cntr, member_func, args...)       \
    ZETA_CallMemberFunc((Zeta_SeqContainer*)ZETA_ToVoidPtr(seq_cntr), \
                        member_func, args)

#define ZETA_SeqContainer_GetWidth(seq_cntr) \
    ZETA_SeqContainer_Call_((seq_cntr), GetWidth)

#define ZETA_SeqContainer_GetStride(seq_cntr) \
    ZETA_SeqContainer_Call_((seq_cntr), GetStride)

#define ZETA_SeqContainer_GetSize(seq_cntr) \
    ZETA_SeqContainer_Call_((seq_cntr), GetSize)

#define ZETA_SeqContainer_GetCapacity(seq_cntr) \
    ZETA_SeqContainer_Call_((seq_cntr), GetCapacity)

#define ZETA_SeqContainer_GetLBCursor(seq_cntr, dst_cursor)         \
    ZETA_SeqContainer_Call_((seq_cntr), GetLBCursor, (dst_cursor)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_GetRBCursor(seq_cntr, dst_cursor)         \
    ZETA_SeqContainer_Call_((seq_cntr), GetRBCursor, (dst_cursor)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_PeekL(seq_cntr, dst_cursor, dst_elem) \
    ZETA_SeqContainer_Call_((seq_cntr), PeekL, (dst_cursor), (dst_elem))

#define ZETA_SeqContainer_PeekR(seq_cntr, dst_cursor, dst_elem) \
    ZETA_SeqContainer_Call_((seq_cntr), PeekR, (dst_cursor), (dst_elem))

#define ZETA_SeqContainer_Access(seq_cntr, dst_cursor, dst_elem, idx) \
    ZETA_SeqContainer_Call_((seq_cntr), Access, (dst_cursor), (dst_elem), (idx))

#define ZETA_SeqContainer_Refer(seq_cntr, pos_cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Refer, (pos_cursor))

#define ZETA_SeqContainer_Read(seq_cntr, pos_cursor, cnt, dst, dst_cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Read, (pos_cursor), (cnt), (dst),  \
                            (dst_cursor))

#define ZETA_SeqContainer_Write(seq_cntr, pos_cursor, cnt, src, dst_cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Write, (pos_cursor), (cnt), (src),  \
                            (dst_cursor))

#define ZETA_SeqContainer_PushL(seq_cntr, dst_cursor, cnt) \
    ZETA_SeqContainer_Call_((seq_cntr), PushL, (dst_cursor), (cnt));

#define ZETA_SeqContainer_PushR(seq_cntr, dst_cursor, cnt) \
    ZETA_SeqContainer_Call_((seq_cntr), PushR, (dst_cursor), (cnt));

#define ZETA_SeqContainer_Insert(seq_cntr, pos_cursor, cnt) \
    ZETA_SeqContainer_Call_((seq_cntr), Insert, (pos_cursor), (cnt))

#define ZETA_SeqContainer_PopL(seq_cntr, cnt)         \
    ZETA_SeqContainer_Call_((seq_cntr), PopL, (cnt)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_PopR(seq_cntr, cnt)         \
    ZETA_SeqContainer_Call_((seq_cntr), PopR, (cnt)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_Erase(seq_cntr, pos_cursor, cnt)           \
    ZETA_SeqContainer_Call_((seq_cntr), Erase, (pos_cursor), (cnt)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_EraseAll(seq_cntr)       \
    ZETA_SeqContainer_Call_((seq_cntr), EraseAll); \
    ZETA_StaticAssert(TRUE)

#define ZETA_SeqContainer_Cursor_IsEqual(seq_cntr, cursor_a, cursor_b) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_IsEqual, (cursor_a), (cursor_b))

#define ZETA_SeqContainer_Cursor_Compare(seq_cntr, cursor_a, cursor_b) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_Compare, (cursor_a), (cursor_b))

#define ZETA_SeqContainer_Cursor_GetDist(seq_cntr, cursor_a, cursor_b) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_GetDist, (cursor_a), (cursor_b))

#define ZETA_SeqContainer_Cursor_GetIdx(seq_cntr, cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_GetIdx, (cursor))

#define ZETA_SeqContainer_Cursor_StepL(seq_cntr, cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_StepL, (cursor))

#define ZETA_SeqContainer_Cursor_StepR(seq_cntr, cursor) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_StepR, (cursor))

#define ZETA_SeqContainer_Cursor_AdvanceL(seq_cntr, cursor, idx) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_AdvanceL, (cursor), (idx))

#define ZETA_SeqContainer_Cursor_AdvanceR(seq_cntr, cursor, idx) \
    ZETA_SeqContainer_Call_((seq_cntr), Cursor_AdvanceR, (cursor), (idx))

ZETA_ExternC_End;
