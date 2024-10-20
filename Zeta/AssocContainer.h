#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_AssocContainer);

struct Zeta_AssocContainer {
    void* context;

    size_t cursor_size;

    size_t (*GetWidth)(void* context);

    size_t (*GetStride)(void* context);

    size_t (*GetSize)(void* context);

    size_t (*GetCapacity)(void* context);

    void (*GetLBCursor)(void* context, void* dst_cursor);

    void (*GetRBCursor)(void* context, void* dst_cursor);

    void* (*PeekL)(void* context, void* dst_cursor);

    void* (*PeekR)(void* context, void* dst_cursor);

    void* (*Refer)(void* context, void const* pos_cursor);

    void* (*Find)(void* context, void const* key, void* dst_cursor);

    void* (*Insert)(void* context, void const* elem, void* dst_cursor);

    void (*Erase)(void* context, void* pos_cursor);

    void (*EraseAll)(void* context);

    bool_t (*Cursor_AreEqual)(void* context, void const* cursor_a,
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

void Zeta_AssocContainer_Init(Zeta_AssocContainer* assoc_cntr);

#define ZETA_AssocContainer_AllocaCursor(assoc_cntr)                     \
    __builtin_alloca_with_align(                                         \
        ((Zeta_AssocContainer*)ZETA_ToVoidPtr(assoc_cntr))->cursor_size, \
        CHAR_BIT * alignof(max_align_t))

#define ZETA_AssocContainer_Call_(assoc_cntr, member_func, args...)       \
    ZETA_CallMemberFunc((Zeta_AssocContainer*)ZETA_ToVoidPtr(assoc_cntr), \
                        member_func, args)

#define ZETA_AssocContainer_GetWidth(assoc_cntr) \
    ZETA_AssocContainer_Call_((assoc_cntr), GetWidth)

#define ZETA_AssocContainer_GetStride(assoc_cntr) \
    ZETA_AssocContainer_Call_((assoc_cntr), GetStride)

#define ZETA_AssocContainer_GetSize(assoc_cntr) \
    ZETA_AssocContainer_Call_((assoc_cntr), GetSize)

#define ZETA_AssocContainer_GetCapacity(assoc_cntr) \
    ZETA_AssocContainer_Call_((assoc_cntr), GetCapacity)

#define ZETA_AssocContainer_GetLBCursor(assoc_cntr, dst_cursor)         \
    ZETA_AssocContainer_Call_((assoc_cntr), GetLBCursor, (dst_cursor)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_AssocContainer_GetRBCursor(assoc_cntr, dst_cursor)         \
    ZETA_AssocContainer_Call_((assoc_cntr), GetRBCursor, (dst_cursor)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_AssocContainer_PeekL(assoc_cntr, dst_cursor) \
    ZETA_AssocContainer_Call_((assoc_cntr), PeekL, (dst_cursor))

#define ZETA_AssocContainer_PeekR(assoc_cntr, dst_cursor) \
    ZETA_AssocContainer_Call_((assoc_cntr), PeekR, (dst_cursor))

#define ZETA_AssocContainer_Refer(assoc_cntr, pos_cursor) \
    ZETA_AssocContainer_Call_((assoc_cntr), Refer, (pos_cursor))

#define ZETA_AssocContainer_Find(assoc_cntr, key, dst_cursor) \
    ZETA_AssocContainer_Call_((assoc_cntr), Refer, (key), (dst_cursor))

#define ZETA_AssocContainer_Insert(assoc_cntr, elem, dst_cursor) \
    ZETA_AssocContainer_Call_((assoc_cntr), Insert, (elem), (dst_cursor))

#define ZETA_AssocContainer_Erase(assoc_cntr, pos_cursor)         \
    ZETA_AssocContainer_Call_((assoc_cntr), Erase, (pos_cursor)); \
    ZETA_StaticAssert(TRUE)

#define ZETA_AssocContainer_EraseAll(assoc_cntr)       \
    ZETA_AssocContainer_Call_((assoc_cntr), EraseAll); \
    ZETA_StaticAssert(TRUE)

#define ZETA_AssocContainer_Cursor_AreEqual(assoc_cntr, cursor_a, cursor_b) \
    ZETA_AssocContainer_Call_((assoc_cntr), Cursor_AreEqual, (cursor_a),    \
                              (cursor_b))

#define ZETA_AssocContainer_Cursor_Compare(assoc_cntr, cursor_a, cursor_b) \
    ZETA_AssocContainer_Call_((assoc_cntr), Cursor_Compare, (cursor_a),    \
                              (cursor_b))

#define ZETA_AssocContainer_Cursor_GetDist(assoc_cntr, cursor_a, cursor_b) \
    ZETA_AssocContainer_Call_((assoc_cntr), Cursor_GetDist, (cursor_a),    \
                              (cursor_b))

#define ZETA_AssocContainer_Cursor_GetIdx(assoc_cntr, cursor) \
    ZETA_AssocContainer_Call_((assoc_cntr), Cursor_GetIdx, (cursor))

#define ZETA_AssocContainer_Cursor_StepL(assoc_cntr, cursor) \
    ZETA_AssocContainer_Call_((assoc_cntr), Cursor_StepL, (cursor))

#define ZETA_AssocContainer_Cursor_StepR(assoc_cntr, cursor) \
    ZETA_AssocContainer_Call_((assoc_cntr), Cursor_StepR, (cursor))

#define ZETA_AssocContainer_Cursor_AdvanceL(assoc_cntr, cursor, idx) \
    ZETA_AssocContainer_Call_((assoc_cntr), Cursor_AdvanceL, (cursor), (idx))

#define ZETA_AssocContainer_Cursor_AdvanceR(assoc_cntr, cursor, idx) \
    ZETA_AssocContainer_Call_((assoc_cntr), Cursor_AdvanceR, (cursor), (idx))

ZETA_ExternC_End;
