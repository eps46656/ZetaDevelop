#pragma once

#include "define.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_AssocContainer);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct Zeta_AssocContainer {
    void* context;

    size_t cursor_size;

    size_t (*GetWidth)(void* context);

    size_t (*GetStride)(void* context);

    size_t (*GetSize)(void* context);

    size_t (*GetCapacity)(void* context);

    void (*GetLBCursor)(void* context, void* dst_cursor);

    void (*GetRBCursor)(void* context, void* dst_cursor);

    void (*PeekL)(void* context, void* dst_cursor);

    void (*PeekR)(void* context, void* dst_cursor);

    void* (*Refer)(void* context, void const* pos_cursor);

    void* (*Find)(void* context, void* dst_cursor, void const* key);

    void* (*Insert)(void* context, void* dst_cursor, void const* key);

    void (*Erase)(void* context, void* pos_cursor);

    void (*EraseAll)(void* context);

    void (*Cursor_IsEqual)(void* context, void const* cursor_a,
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

void Zeta_AssocContainer_Init(Zeta_AssocContainer* assoc_cntr);

ZETA_ExternC_End;

/*

Element:
    key part
    val part [may be void]

Find:
    only provide key, to search the element whose key is key.

Insert:


For Tree set
    Compare(elem, key) -> -1, 0, 1

    Require element's key is comparable.

For Hash Table
    functions are provieded by user:
        IsEqual(elem, key) -> TRUE, FALSE
        HashEle(elem) -> ull
        HashKey(key) -> ull

    Require element's key is hashable and differable.

*/
