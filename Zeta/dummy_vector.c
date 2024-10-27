#include "debugger.h"
#include "dummy_vector.h"

#if ZETA_IsDebug

#define CheckDV_(dv) Zeta_DummyVector_Check(dv)

#define CheckDVCursor_(dv, cursor) Zeta_DummyVector_Cursor_Check(dv, cursor)

#else

#define CheckDV_(dv)

#define CheckDVCursor_(dv, cursor)

#endif

size_t Zeta_DummyVector_GetWidth(void* dv_) {
    Zeta_DummyVector* dv = dv_;
    CheckDV_(dv);

    return dv->width;
}

size_t Zeta_DummyVector_GetStride(void* dv_) {
    Zeta_DummyVector* dv = dv_;

    CheckDV_(dv);

    return dv->stride;
}

size_t Zeta_DummyVector_GetSize(void* dv) {
    CheckDV_(dv);
    return 0;
}

size_t Zeta_DummyVector_GetCapacity(void* dv) {
    CheckDV_(dv);
    return 0;
}

void Zeta_DummyVector_GetLBCursor(void* dv, void* dst_cursor) {
    CheckDV_(dv);
    ZETA_DebugAssert(dst_cursor != NULL);
    *(size_t*)dst_cursor = -1;
}

void Zeta_DummyVector_GetRBCursor(void* dv, void* dst_cursor) {
    CheckDV_(dv);
    ZETA_DebugAssert(dst_cursor != NULL);
    *(size_t*)dst_cursor = 0;
}

void* Zeta_DummyVector_PeekL(void* dv, void* dst_cursor, void* dst_elem) {
    ZETA_Unused(dst_elem);

    CheckDV_(dv);
    if (dst_cursor != NULL) { *(size_t*)dst_cursor = 0; }
    return NULL;
}

void* Zeta_DummyVector_PeekR(void* dv, void* dst_cursor, void* dst_elem) {
    ZETA_Unused(dst_elem);

    CheckDV_(dv);
    if (dst_cursor != NULL) { *(size_t*)dst_cursor = -1; }
    return NULL;
}

void* Zeta_DummyVector_Access(void* dv, size_t idx, void* dst_cursor,
                              void* dst_elem) {
    ZETA_Unused(dst_elem);

    CheckDV_(dv);
    ZETA_DebugAssert(idx == (size_t)(-1) || idx == 0);
    if (dst_cursor != NULL) { *(size_t*)dst_cursor = idx; }
    return NULL;
}

void* Zeta_DummyVector_Refer(void* dv, void const* pos_cursor) {
    ZETA_Unused(dv);
    ZETA_Unused(pos_cursor);

    return NULL;
}

void Zeta_DummyVector_Read(void* dv, void const* pos_cursor, size_t cnt,
                           void* dst, void* dst_cursor) {
    CheckDVCursor_(dv, pos_cursor);
    ZETA_DebugAssert(cnt == 0);
    ZETA_Unused(dst);
    ZETA_Unused(dst_cursor);
}

void Zeta_DummyVector_Write(void* dv, void const* pos_cursor, size_t cnt,
                            void const* src, void* dst_cursor) {
    CheckDVCursor_(dv, pos_cursor);
    ZETA_DebugAssert(cnt == 0);
    ZETA_Unused(src);
    ZETA_Unused(dst_cursor);
}

void Zeta_DummyVector_Check(void* dv_) {
    Zeta_DummyVector* dv = dv_;
    ZETA_DebugAssert(dv != NULL);

    ZETA_DebugAssert(0 < dv->width);
    ZETA_DebugAssert(dv->width <= dv->stride);
}

bool_t Zeta_DummyVector_Cursor_AreEqual(void* dv, void const* cursor_a_,
                                        void const* cursor_b_) {
    size_t const* cursor_a = cursor_a_;
    size_t const* cursor_b = cursor_b_;

    CheckDVCursor_(dv, cursor_a);
    CheckDVCursor_(dv, cursor_b);

    return *cursor_a == *cursor_b;
}

int Zeta_DummyVector_Cursor_Compare(void* dv, void const* cursor_a_,
                                    void const* cursor_b_) {
    size_t const* cursor_a = cursor_a_;
    size_t const* cursor_b = cursor_b_;

    CheckDVCursor_(dv, cursor_a);
    CheckDVCursor_(dv, cursor_b);

    if (*cursor_a < *cursor_b) { return -1; }
    if (*cursor_b < *cursor_a) { return 1; }
    return 0;
}

size_t Zeta_DummyVector_Cursor_GetDist(void* dv, void const* cursor_a_,
                                       void const* cursor_b_) {
    size_t const* cursor_a = cursor_a_;
    size_t const* cursor_b = cursor_b_;

    CheckDVCursor_(dv, cursor_a);
    CheckDVCursor_(dv, cursor_b);

    return *cursor_b - *cursor_a;
}

size_t Zeta_DummyVector_Cursor_GetIdx(void* dv, void const* cursor_) {
    size_t const* cursor = cursor_;
    CheckDVCursor_(dv, cursor);

    return *cursor;
}

void Zeta_DummyVector_Cursor_StepL(void* dv, void* cursor) {
    Zeta_DummyVector_Cursor_AdvanceL(dv, cursor, 1);
}

void Zeta_DummyVector_Cursor_StepR(void* dv, void* cursor) {
    Zeta_DummyVector_Cursor_AdvanceR(dv, cursor, 1);
}

void Zeta_DummyVector_Cursor_AdvanceL(void* dv, void* cursor_, size_t step) {
    size_t* cursor = cursor_;
    CheckDVCursor_(dv, cursor);

    ZETA_DebugAssert(step <= *cursor + 1);

    *cursor -= step;
}

void Zeta_DummyVector_Cursor_AdvanceR(void* dv, void* cursor_, size_t step) {
    size_t* cursor = cursor_;
    CheckDVCursor_(dv, cursor);

    ZETA_DebugAssert(step <= -*cursor);

    *cursor += step;
}

void Zeta_DummyVector_Cursor_Check(void* dv, void const* cursor) {
    CheckDV_(dv);
    ZETA_Unused(cursor);
}

void Zeta_DummyVector_DeploySeqCntr(void* dv_, Zeta_SeqCntr* seq_cntr) {
    Zeta_DummyVector* dv = dv_;
    CheckDV_(dv);

    Zeta_SeqCntr_Init(seq_cntr);

    seq_cntr->context = dv;

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

    seq_cntr->Cursor_AreEqual = Zeta_DummyVector_Cursor_AreEqual;

    seq_cntr->Cursor_Compare = Zeta_DummyVector_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_DummyVector_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_DummyVector_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_DummyVector_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_DummyVector_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_DummyVector_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_DummyVector_Cursor_AdvanceR;
}
