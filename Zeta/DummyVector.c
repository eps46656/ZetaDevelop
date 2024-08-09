#include "DummyVector.h"

#include "Debugger.h"

static void CheckDSC_(void* dsc_) {
    Zeta_DummyVector* dsc = dsc_;
    ZETA_DebugAssert(dsc != NULL);

    ZETA_DebugAssert(0 < dsc->width);
    ZETA_DebugAssert(dsc->width <= dsc->stride);
}

static void CheckCursor_(void* dsc_, void const* cursor_) {
    Zeta_DummyVector* dsc = dsc_;
    CheckDSC_(dsc);

    size_t const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ZETA_DebugAssert(*cursor == (size_t)(-1) || *cursor == 0);
}

size_t Zeta_DummyVector_GetWidth(void* dsc_) {
    Zeta_DummyVector* dsc = dsc_;
    CheckDSC_(dsc);

    return dsc->width;
}

size_t Zeta_DummyVector_GetStride(void* dsc_) {
    Zeta_DummyVector* dsc = dsc_;

    CheckDSC_(dsc);

    return dsc->stride;
}

size_t Zeta_DummyVector_GetSize(void* dsc) {
    CheckDSC_(dsc);
    return 0;
}

size_t Zeta_DummyVector_GetCapacity(void* dsc) {
    CheckDSC_(dsc);
    return 0;
}

void Zeta_DummyVector_GetLBCursor(void* dsc, void* dst_cursor) {
    CheckDSC_(dsc);
    ZETA_DebugAssert(dst_cursor != NULL);
    *(size_t*)dst_cursor = -1;
}

void Zeta_DummyVector_GetRBCursor(void* dsc, void* dst_cursor) {
    CheckDSC_(dsc);
    ZETA_DebugAssert(dst_cursor != NULL);
    *(size_t*)dst_cursor = 0;
}

void* Zeta_DummyVector_PeekL(void* dsc, void* dst_cursor, void* dst_elem) {
    ZETA_Unused(dst_elem);

    CheckDSC_(dsc);
    if (dst_cursor != NULL) { *(size_t*)dst_cursor = 0; }
    return NULL;
}

void* Zeta_DummyVector_PeekR(void* dsc, void* dst_cursor, void* dst_elem) {
    ZETA_Unused(dst_elem);

    CheckDSC_(dsc);
    if (dst_cursor != NULL) { *(size_t*)dst_cursor = -1; }
    return NULL;
}

void* Zeta_DummyVector_Access(void* dsc, void* dst_cursor, void* dst_elem,
                              size_t idx) {
    ZETA_Unused(dst_elem);

    CheckDSC_(dsc);
    ZETA_DebugAssert(idx == (size_t)(-1) || idx == 0);
    if (dst_cursor != NULL) { *(size_t*)dst_cursor = idx; }
    return NULL;
}

void* Zeta_DummyVector_Refer(void* dsc, void const* pos_cursor) {
    ZETA_Unused(dsc);
    ZETA_Unused(pos_cursor);

    return NULL;
}

void Zeta_DummyVector_Read(void* dsc, void const* pos_cursor, size_t cnt,
                           void* dst, void* dst_cursor) {
    CheckCursor_(dsc, pos_cursor);
    ZETA_DebugAssert(cnt == 0);
    ZETA_Unused(dst);
    ZETA_Unused(dst_cursor);
}

bool_t Zeta_DummyVector_Cursor_IsEqual(void* dsc, void const* cursor_a_,
                                       void const* cursor_b_) {
    size_t const* cursor_a = cursor_a_;
    size_t const* cursor_b = cursor_b_;

    CheckCursor_(dsc, cursor_a);
    CheckCursor_(dsc, cursor_b);

    return *cursor_a == *cursor_b;
}

int Zeta_DummyVector_Cursor_Compare(void* dsc, void const* cursor_a_,
                                    void const* cursor_b_) {
    size_t const* cursor_a = cursor_a_;
    size_t const* cursor_b = cursor_b_;

    CheckCursor_(dsc, cursor_a);
    CheckCursor_(dsc, cursor_b);

    if (*cursor_a < *cursor_b) { return -1; }
    if (*cursor_b < *cursor_a) { return 1; }
    return 0;
}

size_t Zeta_DummyVector_Cursor_GetDist(void* dsc, void const* cursor_a_,
                                       void const* cursor_b_) {
    size_t const* cursor_a = cursor_a_;
    size_t const* cursor_b = cursor_b_;

    CheckCursor_(dsc, cursor_a);
    CheckCursor_(dsc, cursor_b);

    return *cursor_b - *cursor_a;
}

size_t Zeta_DummyVector_Cursor_GetIdx(void* dsc, void const* cursor_) {
    size_t const* cursor = cursor_;
    CheckCursor_(dsc, cursor);

    return *cursor;
}

void Zeta_DummyVector_Cursor_StepL(void* dsc, void* cursor_) {
    size_t* cursor = cursor_;
    CheckCursor_(dsc, cursor);

    ZETA_DebugAssert(*cursor == (size_t)(-1));

    ++*cursor;
}

void Zeta_DummyVector_Cursor_StepR(void* dsc, void* cursor_) {
    size_t* cursor = cursor_;
    CheckCursor_(dsc, cursor);

    ZETA_DebugAssert(*cursor == 0);

    --*cursor;
}

void Zeta_DummyVector_Cursor_AdvanceL(void* dsc, void* cursor_, size_t step) {
    size_t* cursor = cursor_;
    CheckCursor_(dsc, cursor);

    ZETA_DebugAssert(*cursor - step + 1 <= 1);

    *cursor -= step;
}

void Zeta_DummyVector_Cursor_AdvanceR(void* dsc, void* cursor_, size_t step) {
    size_t* cursor = cursor_;
    CheckCursor_(dsc, cursor);

    ZETA_DebugAssert(*cursor + step + 1 <= 1);

    *cursor += step;
}

void Zeta_DummyVector_DeploySeqContainer(void* dsc_,
                                         Zeta_SeqContainer* seq_cntr) {
    Zeta_DummyVector* dsc = dsc_;
    CheckDSC_(dsc);

    ZETA_DebugAssert(seq_cntr != NULL);

    Zeta_SeqContainer_Init(seq_cntr);

    seq_cntr->context = dsc;

    seq_cntr->cursor_size = sizeof(size_t);

    seq_cntr->GetWidth = Zeta_DummyVector_GetWidth;

    seq_cntr->GetStride = Zeta_DummyVector_GetStride;

    seq_cntr->GetSize = Zeta_DummyVector_GetSize;

    seq_cntr->GetCapacity = Zeta_DummyVector_GetCapacity;

    seq_cntr->GetLBCursor = Zeta_DummyVector_GetLBCursor;

    seq_cntr->GetRBCursor = Zeta_DummyVector_GetRBCursor;

    seq_cntr->PeekL = Zeta_DummyVector_PeekL;

    seq_cntr->PeekR = Zeta_DummyVector_PeekR;

    seq_cntr->Access = Zeta_DummyVector_Access;

    seq_cntr->Refer = Zeta_DummyVector_Refer;

    seq_cntr->Read = Zeta_DummyVector_Read;

    seq_cntr->Cursor_IsEqual = Zeta_DummyVector_Cursor_IsEqual;

    seq_cntr->Cursor_Compare = Zeta_DummyVector_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_DummyVector_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_DummyVector_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_DummyVector_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_DummyVector_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_DummyVector_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_DummyVector_Cursor_AdvanceR;
}
