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

    void* (*Find)(void* context, void* dst_cursor, void const* key);

    void* (*Insert)(void* context, void* dst_cursor, void const* key);

    void (*Erase)(void* context, void* pos_cursor);

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

void Zeta_AssocContainer_Init(Zeta_AssocContainer* assoc_cntr);

#define ZETA_AssocContainer_AllocaCursor_(assoc_cntr_tmp, assoc_cntr) \
    ({                                                                \
        Zeta_AssocContainer const* assoc_cntr_tmp = (assoc_cntr);     \
        __builtin_alloca_with_align(assoc_cntr_tmp->cursor_size,      \
                                    CHAR_BIT * alignof(max_align_t)); \
    })

#define ZETA_AssocContainer_AllocaCursor(assoc_cntr) \
    ZETA_AssocContainer_AllocaCursor_(ZETA_TmpeName, (assoc_cntr))

ZETA_ExternC_End;
