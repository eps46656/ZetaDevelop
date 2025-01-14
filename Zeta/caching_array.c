

#include "caching_array.h"

#include "debugger.h"

#if ZETA_EnableDebug

#define CheckCntr_(ca) Zeta_CachingArray_Check((ca))

#define CheckCursor_(ca, cursor) Zeta_CachingArray_Cursor_Check((ca), (cursor))

#else

#define CheckCntr_(ca) ZETA_Unused((ca))

#define CheckCursor_(ca, cursor) \
    {                            \
        ZETA_Unused((ca));       \
        ZETA_Unused((cursor));   \
    }                            \
    ZETA_StaticAssert(TRUE)

#endif

void Zeta_CachingArray_Init(void* ca_) {
    Zeta_CachingArray* ca = ca_;
    ZETA_DebugAssert(ca != NULL);

    ZETA_DebugAssert(ca->cm != NULL);

    ca->origin = ZETA_CacheManager_GetOrigin(ca->cm);
}

void Zeta_CachingArray_Denit(void* ca_) {
    Zeta_CachingArray* ca = ca_;
    CheckCntr_(ca);

    //
}

size_t Zeta_CachingArray_GetWidth(void const* ca_) {
    Zeta_CachingArray const* ca = ca_;
    CheckCntr_(ca);

    return ZETA_SeqCntr_GetWidth(ca->origin);
}

size_t Zeta_CachingArray_GetSize(void const* ca_) {
    Zeta_CachingArray const* ca = ca_;
    CheckCntr_(ca);

    return ZETA_SeqCntr_GetSize(ca->origin);
}

size_t Zeta_CachingArray_GetCapacity(void const* ca_) {
    Zeta_CachingArray const* ca = ca_;
    CheckCntr_(ca);

    return ZETA_SeqCntr_GetCapacity(ca->origin);
}

void Zeta_CachingArray_GetLBCursor(void const* ca_, void* dst_cursor);

void Zeta_CachingArray_GetRBCursor(void const* ca_, void* dst_cursor);

void* Zeta_CachingArray_PeekL(void* ca_, void* dst_cursor, void* dst_elem) {
    Zeta_CachingArray* ca = ca_;
    CheckCntr_(ca);

    return Zeta_CachingArray_Access(ca, 0, dst_cursor, dst_elem);
}

void* Zeta_CachingArray_PeekR(void* ca_, void* dst_cursor, void* dst_elem) {
    Zeta_CachingArray* ca = ca_;
    CheckCntr_(ca);

    Zeta_CachingArray_Access(ca, 0, dst_cursor, dst_elem);

    return Zeta_CachingArray_Access(ca, ZETA_SeqCntr_GetSize(ca->origin) - 1,
                                    dst_cursor, dst_elem);
}

void* Zeta_CachingArray_Access(void* ca_, size_t idx, void* dst_cursor_,
                               void* dst_elem) {
    Zeta_CachingArray* ca = ca_;
    CheckCntr_(ca);

    Zeta_CachingArray_Cursor* dst_cursor = dst_cursor_;

    size_t size = ZETA_SeqCntr_GetSize(ca->origin);

    ZETA_DebugAssert(idx + 1 < size + 2);

    if (dst_cursor != NULL) {
        dst_cursor->ca = ca;
        dst_cursor->idx = idx;
    }

    if (dst_elem != NULL && (idx != (size_t)(-1) && idx != size)) {
        ZETA_CacheManager_Read(ca->cm, ca->sd, idx, 1, dst_elem, ZETA_SIZE_MAX);
    }

    return NULL;
}

void* Zeta_CachingArray_Refer(void* ca_, void const* pos_cursor_) {
    Zeta_CachingArray* ca = ca_;
    Zeta_CachingArray_Cursor const* pos_cursor = pos_cursor_;

    CheckCursor_(ca, pos_cursor);

    return NULL;
}

void Zeta_CachingArray_Read(void const* ca_, void const* pos_cursor_,
                            size_t cnt, void* dst, size_t dst_stride,
                            void* dst_cursor_) {
    Zeta_CachingArray const* ca = ca_;
    Zeta_CachingArray_Cursor const* pos_cursor = pos_cursor_;

    CheckCursor_(ca, pos_cursor);

    Zeta_CachingArray_Cursor* dst_cursor = dst_cursor_;

    size_t idx = pos_cursor->idx;

    ZETA_DebugAssert(cnt <= ZETA_SeqCntr_GetSize(ca->origin) - idx);

    ZETA_CacheManager_Read(ca->cm, ca->sd, idx, cnt, dst, dst_stride);

    if (dst_cursor != NULL) {
        dst_cursor->ca = ca;
        dst_cursor->idx = idx + cnt;
    }
}

void Zeta_CachingArray_Write(void* ca_, void* pos_cursor_, size_t cnt,
                             void const* src, size_t src_stride,
                             void* dst_cursor_) {
    Zeta_CachingArray* ca = ca_;
    Zeta_CachingArray_Cursor* pos_cursor = pos_cursor_;

    CheckCursor_(ca, pos_cursor);

    Zeta_CachingArray_Cursor* dst_cursor = dst_cursor_;

    size_t idx = pos_cursor->idx;

    ZETA_DebugAssert(cnt <= ZETA_SeqCntr_GetSize(ca->origin) - idx);

    ZETA_CacheManager_Write(ca->cm, ca->sd, idx, cnt, src, src_stride);

    if (dst_cursor != NULL) {
        dst_cursor->ca = ca;
        dst_cursor->idx = idx + cnt;
    }
}

void Zeta_CachingArray_Check(void const* ca_) {
    Zeta_CachingArray const* ca = ca_;
    ZETA_DebugAssert(ca != NULL);

    ZETA_DebugAssert(ca->cm != NULL);

    ZETA_DebugAssert(ca->origin == ZETA_CacheManager_GetOrigin(ca->cm));
}

bool_t Zeta_CachingArray_Cursor_AreEqual(void const* ca_, void const* cursor_a_,
                                         void const* cursor_b_) {
    Zeta_CachingArray const* ca = ca_;
    Zeta_CachingArray_Cursor const* cursor_a = cursor_a_;
    Zeta_CachingArray_Cursor const* cursor_b = cursor_b_;

    CheckCursor_(ca, cursor_a_);
    CheckCursor_(ca, cursor_b_);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_CachingArray_Cursor_Compare(void const* ca_, void const* cursor_a_,
                                     void const* cursor_b_) {
    Zeta_CachingArray const* ca = ca_;
    Zeta_CachingArray_Cursor const* cursor_a = cursor_a_;
    Zeta_CachingArray_Cursor const* cursor_b = cursor_b_;

    CheckCursor_(ca, cursor_a_);
    CheckCursor_(ca, cursor_b_);

    return ZETA_ThreeWayCompare(cursor_a->idx, cursor_b->idx);
}

size_t Zeta_CachingArray_Cursor_GetDist(void const* ca_, void const* cursor_a_,
                                        void const* cursor_b_) {
    Zeta_CachingArray const* ca = ca_;
    Zeta_CachingArray_Cursor const* cursor_a = cursor_a_;
    Zeta_CachingArray_Cursor const* cursor_b = cursor_b_;

    CheckCursor_(ca, cursor_a_);
    CheckCursor_(ca, cursor_b_);

    return cursor_b->idx - cursor_a->idx;
}

size_t Zeta_CachingArray_Cursor_GetIdx(void const* ca_, void const* cursor_) {
    Zeta_CachingArray const* ca = ca_;
    Zeta_CachingArray_Cursor const* cursor = cursor_;

    CheckCursor_(ca, cursor);

    return cursor->idx;
}

void Zeta_CachingArray_Cursor_StepL(void const* ca, void* cursor) {
    Zeta_CachingArray_Cursor_AdvanceL(ca, cursor, 1);
}

void Zeta_CachingArray_Cursor_StepR(void const* ca, void* cursor) {
    Zeta_CachingArray_Cursor_AdvanceR(ca, cursor, 1);
}

void Zeta_CachingArray_Cursor_AdvanceL(void const* ca_, void* cursor_,
                                       size_t step) {
    Zeta_CachingArray const* ca = ca_;
    Zeta_CachingArray_Cursor* cursor = cursor_;

    CheckCursor_(ca, cursor);

    ZETA_DebugAssert(step <= cursor->idx + 1);

    cursor->idx -= step;
}

void Zeta_CachingArray_Cursor_AdvanceR(void const* ca_, void* cursor_,
                                       size_t step) {
    Zeta_CachingArray const* ca = ca_;
    Zeta_CachingArray_Cursor* cursor = cursor_;

    CheckCursor_(ca, cursor);

    ZETA_DebugAssert(step <= ZETA_SeqCntr_GetSize(ca->origin) - cursor->idx);

    cursor->idx += step;
}

void Zeta_CachingArray_Cursor_Check(void const* ca_, void const* cursor_) {
    Zeta_CachingArray const* ca = ca_;
    CheckCntr_(ca);

    Zeta_CachingArray_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ZETA_DebugAssert(cursor->ca == ca);
    ZETA_DebugAssert(cursor->idx + 1 < ZETA_SeqCntr_GetSize(ca->origin) + 2);
}

void Zeta_CachingArray_DeploySeqCntr(void* ca_, Zeta_SeqCntr* seq_cntr) {
    Zeta_CachingArray* ca = ca_;
    CheckCntr_(ca);

    ZETA_DebugAssert(seq_cntr != NULL);

    Zeta_SeqCntr_Init(seq_cntr);

    seq_cntr->context = ca;

    seq_cntr->const_context = ca;

    seq_cntr->cursor_size = sizeof(Zeta_CachingArray_Cursor);

    seq_cntr->GetWidth = Zeta_CachingArray_GetWidth;

    seq_cntr->GetSize = Zeta_CachingArray_GetSize;

    seq_cntr->GetCapacity = Zeta_CachingArray_GetCapacity;

    seq_cntr->GetLBCursor = Zeta_CachingArray_GetLBCursor;

    seq_cntr->GetRBCursor = Zeta_CachingArray_GetRBCursor;

    seq_cntr->PeekL = Zeta_CachingArray_PeekL;

    seq_cntr->PeekR = Zeta_CachingArray_PeekR;

    seq_cntr->Access = Zeta_CachingArray_Access;

    seq_cntr->Refer = Zeta_CachingArray_Refer;

    seq_cntr->Read = Zeta_CachingArray_Read;

    seq_cntr->Write = Zeta_CachingArray_Write;

    seq_cntr->Cursor_AreEqual = Zeta_CachingArray_Cursor_AreEqual;

    seq_cntr->Cursor_Compare = Zeta_CachingArray_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_CachingArray_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_CachingArray_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_CachingArray_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_CachingArray_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_CachingArray_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_CachingArray_Cursor_AdvanceR;
}

ZETA_ExternC_End;
