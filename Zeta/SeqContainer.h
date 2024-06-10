#pragma once

#include "define.h"

ZETA_DeclareStruct(Zeta_SeqContainer);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct Zeta_SeqContainer {
    void* context;

    size_t cursor_width;

    size_t (*GetWidth)(void* context);

    size_t (*GetStride)(void* context);

    size_t (*GetSize)(void* context);

    void (*GetLBCursor)(void* context, void* dst_cursor);

    void (*GetRBCursor)(void* context, void* dst_cursor);

    void* (*PeekL)(void* context, void* dst_cursor, void* dst_ele);

    void* (*PeekR)(void* context, void* dst_cursor, void* dst_ele);

    void* (*Access)(void* context, void* dst_cursor, void* dst_ele, size_t idx);

    void* (*Refer)(void* context, void* pos_cursor);

    void (*Read)(void* context, void* pos_cursor, size_t cnt, void* dst,
                 void* dst_cursor);

    void (*Write)(void* context, void* pos_cursor, size_t cnt, void const* src,
                  void* dst_cursor);

    void* (*PushL)(void* context, void* dst_cursor);

    void* (*PushR)(void* context, void* dst_cursor);

    void* (*Insert)(void* context, void* pos_cursor, size_t cnt);

    void (*PopL)(void* context);

    void (*PopR)(void* context);

    void (*Erase)(void* context, void* pos_cursor, size_t cnt);

    void (*EraseAll)(void* context, void* callback_context,
                     void (*Callback)(void* context, void* ele));

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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_SeqContainer_Init(Zeta_SeqContainer* seq_cntr);
