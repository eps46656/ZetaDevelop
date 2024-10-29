#include "assoc_cntr.h"

#include "debugger.h"

void Zeta_AssocCntr_Init(Zeta_AssocCntr* assoc_cntr) {
    ZETA_DebugAssert(assoc_cntr != NULL);

    assoc_cntr->context = NULL;

    assoc_cntr->cursor_size = 0;

    assoc_cntr->GetWidth = NULL;

    assoc_cntr->GetStride = NULL;

    assoc_cntr->GetSize = NULL;

    assoc_cntr->GetCapacity = NULL;

    assoc_cntr->GetLBCursor = NULL;

    assoc_cntr->GetRBCursor = NULL;

    assoc_cntr->PeekL = NULL;

    assoc_cntr->PeekR = NULL;

    assoc_cntr->Refer = NULL;

    assoc_cntr->Find = NULL;

    assoc_cntr->Insert = NULL;

    assoc_cntr->Erase = NULL;

    assoc_cntr->EraseAll = NULL;

    assoc_cntr->Cursor_AreEqual = NULL;

    assoc_cntr->Cursor_Compare = NULL;

    assoc_cntr->Cursor_GetDist = NULL;

    assoc_cntr->Cursor_GetIdx = NULL;

    assoc_cntr->Cursor_StepL = NULL;

    assoc_cntr->Cursor_StepR = NULL;

    assoc_cntr->Cursor_AdvanceL = NULL;

    assoc_cntr->Cursor_AdvanceR = NULL;
}

/*

Element:
    key part
    val part [may be void]

Find:
    only provide key, to search the element whose key is key.

Insert:


For Tree set
    Compare(elem_ref, key_ref) -> -1, 0, 1

    Require element's key is comparable.

For Hash Table
    functions are provieded by user:
        IsEqual(elem_ref, key_ref) -> TRUE, FALSE
        HashEle(elem_ref) -> ull
        HashKey(key_ref) -> ull

    Require element's key is hashable and differable.

*/
