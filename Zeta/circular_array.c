#include "circular_array.h"

#include "debugger.h"
#include "utils.h"

#if ZETA_EnableDebug

#define CheckCntr_(ca) Zeta_CircularArray_Check((ca))

#define CheckCursor_(ca, cursor) Zeta_CircularArray_Cursor_Check((ca), (cursor))

#else

#define CheckCntr_(ca) ZETA_Unused((ca))

#define CheckCursor_(ca, cursor) \
    {                            \
        ZETA_Unused((ca));       \
        ZETA_Unused((cursor));   \
    }                            \
    ZETA_StaticAssert(TRUE)

#endif

#define Refer_(data, stride, offset, idx, capacity) \
    ((data) + (stride) * (((offset) + (idx)) % (capacity)))

void Zeta_CircularArray_Init(void* ca_) {
    Zeta_CircularArray* ca = ca_;
    CheckCntr_(ca);
}

void Zeta_CircularArray_Deinit(void* ca) { ZETA_Unused(ca); }

size_t Zeta_CircularArray_GetWidth(void const* ca_) {
    Zeta_CircularArray const* ca = ca_;
    CheckCntr_(ca);

    return ca->width;
}

size_t Zeta_CircularArray_GetSride(void const* ca_) {
    Zeta_CircularArray const* ca = ca_;
    CheckCntr_(ca);

    return ca->stride;
}

size_t Zeta_CircularArray_GetOffset(void const* ca_) {
    Zeta_CircularArray const* ca = ca_;
    CheckCntr_(ca);

    return ca->offset;
}

size_t Zeta_CircularArray_GetSize(void const* ca_) {
    Zeta_CircularArray const* ca = ca_;
    CheckCntr_(ca);

    return ca->size;
}

size_t Zeta_CircularArray_GetCapacity(void const* ca_) {
    Zeta_CircularArray const* ca = ca_;
    CheckCntr_(ca);

    return ca->capacity;
}

void Zeta_CircularArray_GetLBCursor(void const* ca_, void* dst_cursor_) {
    Zeta_CircularArray const* ca = ca_;
    CheckCntr_(ca);

    Zeta_CircularArray_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->ca = ca;
    dst_cursor->idx = -1;
    dst_cursor->ref = NULL;
}

void Zeta_CircularArray_GetRBCursor(void const* ca_, void* dst_cursor_) {
    Zeta_CircularArray const* ca = ca_;
    CheckCntr_(ca);

    Zeta_CircularArray_Cursor* dst_cursor = dst_cursor_;

    if (dst_cursor == NULL) { return; }

    dst_cursor->ca = ca;
    dst_cursor->idx = ca->size;
    dst_cursor->ref = NULL;
}

void* Zeta_CircularArray_PeekL(void* ca_, void* dst_cursor_, void* dst_elem) {
    Zeta_CircularArray* ca = ca_;
    CheckCntr_(ca);

    Zeta_CircularArray_Cursor* dst_cursor = dst_cursor_;

    void* data = ca->data;
    size_t width = ca->width;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    void* ref = 0 < size ? Refer_(data, stride, offset, 0, capacity) : NULL;

    if (dst_cursor != NULL) {
        dst_cursor->ca = ca;
        dst_cursor->idx = 0;
        dst_cursor->ref = ref;
    }

    if (ref != NULL && dst_elem != NULL) { Zeta_MemCopy(dst_elem, ref, width); }

    return ref;
}

void* Zeta_CircularArray_PeekR(void* ca_, void* dst_cursor_, void* dst_elem) {
    Zeta_CircularArray* ca = ca_;
    CheckCntr_(ca);

    Zeta_CircularArray_Cursor* dst_cursor = dst_cursor_;

    void* data = ca->data;
    size_t width = ca->width;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    void* ref =
        0 < size ? Refer_(data, stride, offset, size - 1, capacity) : NULL;

    if (dst_cursor != NULL) {
        dst_cursor->ca = ca;
        dst_cursor->idx = size - 1;
        dst_cursor->ref = ref;
    }

    if (ref != NULL && dst_elem != NULL) { Zeta_MemCopy(dst_elem, ref, width); }

    return ref;
}

void* Zeta_CircularArray_Access(void* ca_, size_t idx, void* dst_cursor_,
                                void* dst_elem) {
    Zeta_CircularArray* ca = ca_;
    CheckCntr_(ca);

    Zeta_CircularArray_Cursor* dst_cursor = dst_cursor_;

    void* data = ca->data;
    size_t width = ca->width;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    ZETA_DebugAssert(idx + 1 < size + 2);

    void* ref = idx < size ? Refer_(data, stride, offset, idx, capacity) : NULL;

    if (dst_cursor != NULL) {
        dst_cursor->ca = ca;
        dst_cursor->idx = idx;
        dst_cursor->ref = ref;
    }

    if (ref != NULL && dst_elem != NULL) { Zeta_MemCopy(dst_elem, ref, width); }

    return ref;
}

void* Zeta_CircularArray_Refer(void* ca_, void const* pos_cursor_) {
    Zeta_CircularArray* ca = ca_;
    Zeta_CircularArray_Cursor const* pos_cursor = pos_cursor_;

    CheckCursor_(ca, pos_cursor);

    return pos_cursor->ref;
}

void Zeta_CircularArray_Read(void const* ca_, void const* pos_cursor_,
                             size_t cnt, void* dst, size_t dst_stride,
                             void* dst_cursor_) {
    Zeta_CircularArray const* ca = ca_;
    Zeta_CircularArray_Cursor const* pos_cursor = pos_cursor_;

    CheckCursor_(ca, pos_cursor);

    Zeta_CircularArray_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) { return; }

    ZETA_DebugAssert(dst != NULL);

    void* data = ca->data;
    size_t width = ca->width;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    size_t idx = pos_cursor->idx;

    ZETA_DebugAssert(idx <= size);
    ZETA_DebugAssert(cnt <= size - idx);

    while (0 < cnt) {
        size_t cur_cnt =
            ZETA_GetMinOf(cnt, Zeta_CircularArray_GetLongestContSucr(ca, idx));
        cnt -= cur_cnt;

        Zeta_ElemCopy(dst, Refer_(data, stride, offset, idx, capacity), width,
                      dst_stride, stride, cur_cnt);

        idx += cur_cnt;
        dst += dst_stride * cur_cnt;
    }

    if (dst_cursor != NULL) {
        dst_cursor->ca = ca;
        dst_cursor->idx = idx;
        dst_cursor->ref =
            idx == size ? NULL : Refer_(data, stride, offset, idx, capacity);
    }
}

void Zeta_CircularArray_Write(void* ca_, void* pos_cursor_, size_t cnt,
                              void const* src, size_t src_stride,
                              void* dst_cursor_) {
    Zeta_CircularArray* ca = ca_;
    Zeta_CircularArray_Cursor* pos_cursor = pos_cursor_;

    CheckCursor_(ca, pos_cursor);

    Zeta_CircularArray_Cursor* dst_cursor = dst_cursor_;

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->ca = ca;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->ref = pos_cursor->ref;
        }

        return;
    }

    ZETA_DebugAssert(src != NULL);

    void* data = ca->data;
    size_t width = ca->width;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    size_t idx = pos_cursor->idx;

    ZETA_DebugAssert(idx <= size);
    ZETA_DebugAssert(cnt <= size - idx);

    while (0 < cnt) {
        size_t cur_cnt =
            ZETA_GetMinOf(cnt, Zeta_CircularArray_GetLongestContSucr(ca, idx));
        cnt -= cur_cnt;

        Zeta_ElemCopy(Refer_(data, stride, offset, idx, capacity), src, width,
                      stride, src_stride, cur_cnt);

        idx += cur_cnt;
        src += src_stride * cur_cnt;
    }

    if (dst_cursor != NULL) {
        dst_cursor->ca = ca;
        dst_cursor->idx = idx;
        dst_cursor->ref =
            idx == size ? NULL : Refer_(data, stride, offset, idx, capacity);
    }
}

void* Zeta_CircularArray_PushL(void* ca_, size_t cnt, void* dst_cursor_) {
    Zeta_CircularArray* ca = ca_;
    CheckCntr_(ca);

    Zeta_CircularArray_Cursor* dst_cursor = dst_cursor_;

    void* data = ca->data;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    ZETA_DebugAssert(cnt <= capacity - size);

    ca->offset = offset = (offset + capacity - cnt) % capacity;
    ca->size = size += cnt;

    void* ref = Refer_(data, stride, offset, 0, capacity);

    if (dst_cursor != NULL) {
        dst_cursor->ca = ca;
        dst_cursor->idx = 0;
        dst_cursor->ref = ref;
    }

    return ref;
}

void* Zeta_CircularArray_PushR(void* ca_, size_t cnt, void* dst_cursor_) {
    Zeta_CircularArray* ca = ca_;
    CheckCntr_(ca);

    Zeta_CircularArray_Cursor* dst_cursor = dst_cursor_;

    void* data = ca->data;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    ZETA_DebugAssert(cnt <= capacity - size);

    ca->size = size += cnt;

    void* ref =
        cnt == 0 ? NULL : Refer_(data, stride, offset, size - cnt, capacity);

    if (dst_cursor != NULL) {
        dst_cursor->ca = ca;
        dst_cursor->idx = size;
        dst_cursor->ref = ref;
    }

    return ref;
}

void* Zeta_CircularArray_Insert(void* ca_, void* pos_cursor_, size_t cnt) {
    Zeta_CircularArray* ca = ca_;
    Zeta_CircularArray_Cursor* pos_cursor = pos_cursor_;

    CheckCursor_(ca, pos_cursor);

    void* data = ca->data;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    size_t idx = pos_cursor->idx;

    ZETA_SeqCntr_CheckInsertable(idx, cnt, size, capacity);

    size_t l_size = idx;
    size_t r_size = size - idx;

    ca->size = size += cnt;

    unsigned long long random_seed = Zeta_GetRandom();

    if (Zeta_Choose2(l_size <= r_size, r_size <= l_size, &random_seed) == 0) {
        ca->offset = offset = (offset + capacity - cnt) % capacity;
        Zeta_CircularArray_Assign(ca, ca, 0, cnt, l_size);
    } else {
        Zeta_CircularArray_Assign(ca, ca, l_size + cnt, l_size, r_size);
    }

    pos_cursor->ref = Refer_(data, stride, offset, idx, capacity);

    return pos_cursor->ref;
}

void Zeta_CircularArray_PopL(void* ca_, size_t cnt) {
    Zeta_CircularArray* ca = ca_;
    CheckCntr_(ca);

    size_t size = ca->size;
    ZETA_DebugAssert(cnt <= size);

    ca->offset = (ca->offset + cnt) % ca->capacity;
    ca->size = size -= cnt;

    if (size == 0) { ca->offset = 0; }
}

void Zeta_CircularArray_PopR(void* ca_, size_t cnt) {
    Zeta_CircularArray* ca = ca_;
    CheckCntr_(ca);

    size_t size = ca->size;
    ZETA_DebugAssert(cnt <= size);

    ca->size = size -= cnt;

    if (size == 0) { ca->offset = 0; }
}

void Zeta_CircularArray_Erase(void* ca_, void* pos_cursor_, size_t cnt) {
    Zeta_CircularArray* ca = ca_;
    Zeta_CircularArray_Cursor* pos_cursor = pos_cursor_;

    CheckCursor_(ca, pos_cursor);

    void* data = ca->data;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    size_t idx = pos_cursor->idx;

    ZETA_SeqCntr_CheckErasable(idx, cnt, size);

    if (cnt == 0) { return; }

    size_t l_size = idx;
    size_t r_size = size - idx - cnt;

    unsigned long long random_seed = Zeta_GetRandom();

    if (Zeta_Choose2(l_size <= r_size, r_size <= l_size, &random_seed)) {
        Zeta_CircularArray_Assign(ca, ca, cnt, 0, l_size);
        ca->offset = offset = (offset + cnt) % capacity;
    } else {
        Zeta_CircularArray_Assign(ca, ca, l_size, l_size + cnt, r_size);
    }

    ca->size = (size -= cnt);

    if (size == 0) { ca->offset = offset = 0; }

    pos_cursor->ref =
        idx < size ? Refer_(data, stride, offset, idx, capacity) : NULL;
}

void Zeta_CircularArray_EraseAll(void* ca_) {
    Zeta_CircularArray* ca = ca_;
    CheckCntr_(ca);

    ca->offset = 0;
    ca->size = 0;
}

void Zeta_CircularArray_Assign(void* dst_ca_, void* src_ca_, size_t dst_beg,
                               size_t src_beg, size_t cnt) {
    Zeta_CircularArray* dst_ca = dst_ca_;
    Zeta_CircularArray* src_ca = src_ca_;

    CheckCntr_(dst_ca);
    CheckCntr_(src_ca);

    void* dst_data = dst_ca->data;
    size_t dst_width = dst_ca->width;
    size_t dst_stride = dst_ca->stride;
    size_t dst_offset = dst_ca->offset;
    size_t dst_size = dst_ca->size;
    size_t dst_capacity = dst_ca->capacity;

    void* src_data = src_ca->data;
    size_t src_width = src_ca->width;
    size_t src_stride = src_ca->stride;
    size_t src_offset = src_ca->offset;
    size_t src_size = src_ca->size;
    size_t src_capacity = src_ca->capacity;

    ZETA_DebugAssert(dst_beg <= dst_size);
    ZETA_DebugAssert(src_beg <= src_size);

    if (cnt == 0) { return; }

    size_t width = ZETA_GetMinOf(dst_width, src_width);

    ZETA_DebugAssert(cnt <= dst_size - dst_beg);
    ZETA_DebugAssert(cnt <= src_size - src_beg);

    if (dst_ca != src_ca) {
        ZETA_DebugAssert(!ZETA_AreOverlapped(dst_data, dst_data + dst_capacity,
                                             src_data,
                                             src_data + src_capacity));

        goto VEC_MOVE;
    }

    if (dst_beg == src_beg) { return; }

    if (dst_beg < src_beg) {
        goto VEC_FW_MOVE;
    } else {
        goto VEC_BW_MOVE;
    }

VEC_MOVE:

VEC_FW_MOVE: {
    while (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(
            cnt, ZETA_GetMinOf(
                     Zeta_CircularArray_GetLongestContSucr(dst_ca, dst_beg),
                     Zeta_CircularArray_GetLongestContSucr(src_ca, src_beg)));
        cnt -= cur_cnt;

        Zeta_ElemMove(
            Refer_(dst_data, dst_stride, dst_offset, dst_beg, dst_capacity),
            Refer_(src_data, src_stride, src_offset, src_beg, src_capacity),
            width, dst_stride, src_stride, cur_cnt);

        dst_beg += cur_cnt;
        src_beg += cur_cnt;
    }

    return;
}

VEC_BW_MOVE: {
    for (size_t dst_end = dst_beg + cnt, src_end = src_beg + cnt; 0 < cnt;) {
        size_t cur_cnt = ZETA_GetMinOf(
            cnt, ZETA_GetMinOf(
                     Zeta_CircularArray_GetLongestContPred(dst_ca, dst_end),
                     Zeta_CircularArray_GetLongestContPred(src_ca, src_end)));
        cnt -= cur_cnt;

        dst_end -= cur_cnt;
        src_end -= cur_cnt;

        Zeta_ElemMove(
            Refer_(dst_data, dst_stride, dst_offset, dst_end, dst_capacity),
            Refer_(src_data, src_stride, src_offset, src_end, src_capacity),
            width, dst_stride, src_stride, cur_cnt);
    }

    return;
}
}

void Zeta_CircularArray_AssignFromSeqCntr(void* ca_, void* ca_cursor_,
                                          Zeta_SeqCntr seq_cntr,
                                          void* seq_cntr_cursor, size_t cnt) {
    Zeta_CircularArray* ca = ca_;
    Zeta_CircularArray_Cursor* ca_cursor = ca_cursor_;

    CheckCursor_(ca, ca_cursor);

    if (seq_cntr.vtable == &zeta_circular_array_seq_cntr_vtable) {
        CheckCursor_(seq_cntr.context, seq_cntr_cursor);

        Zeta_CircularArray_Assign(
            ca, seq_cntr.context, ((Zeta_CircularArray_Cursor*)ca_cursor)->idx,
            ((Zeta_CircularArray_Cursor*)seq_cntr_cursor)->idx, cnt);

        return;
    }

    void* data = ca->data;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    size_t idx = ((Zeta_CircularArray_Cursor*)ca_cursor)->idx;

    ZETA_DebugAssert(idx <= size);
    ZETA_DebugAssert(cnt <= size - idx);

    ZETA_DebugAssert(seq_cntr.vtable->Read != NULL);

    while (0 < cnt) {
        size_t cur_cnt =
            ZETA_GetMinOf(cnt, Zeta_CircularArray_GetLongestContSucr(ca, idx));
        cnt -= cur_cnt;

        ZETA_SeqCntr_Read(seq_cntr, seq_cntr_cursor, cur_cnt,
                          Refer_(data, stride, offset, idx, capacity), stride,
                          seq_cntr_cursor);

        idx += cur_cnt;
    }
}

size_t Zeta_CircularArray_GetLongestContPred(void const* ca_, size_t idx) {
    Zeta_CircularArray const* ca = ca_;
    CheckCntr_(ca);

    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    ZETA_DebugAssert(idx <= size);

    size_t k = capacity - offset;

    return idx <= k ? idx : idx - k;
}

size_t Zeta_CircularArray_GetLongestContSucr(void const* ca_, size_t idx) {
    Zeta_CircularArray const* ca = ca_;
    CheckCntr_(ca);

    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    ZETA_DebugAssert(idx <= size);

    size_t k = capacity - offset;

    return (size <= k || k <= idx) ? size - idx : k - idx;
}

void Zeta_CircularArray_Check(void const* ca_) {
    Zeta_CircularArray const* ca = ca_;
    ZETA_DebugAssert(ca != NULL);

    void* data = ca->data;
    size_t width = ca->width;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);
    ZETA_DebugAssert(offset == 0 || offset < capacity);
    ZETA_DebugAssert(size <= capacity);
    ZETA_DebugAssert(capacity <= ZETA_max_capacity);

    if (data == NULL) { ZETA_DebugAssert(capacity == 0); }
}

bool_t Zeta_CircularArray_Cursor_AreEqual(void const* ca_,
                                          void const* cursor_a_,
                                          void const* cursor_b_) {
    Zeta_CircularArray const* ca = ca_;
    Zeta_CircularArray_Cursor const* cursor_a = cursor_a_;
    Zeta_CircularArray_Cursor const* cursor_b = cursor_b_;

    CheckCursor_(ca, cursor_a);
    CheckCursor_(ca, cursor_b);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_CircularArray_Cursor_Compare(void const* ca_, void const* cursor_a_,
                                      void const* cursor_b_) {
    Zeta_CircularArray const* ca = ca_;
    Zeta_CircularArray_Cursor const* cursor_a = cursor_a_;
    Zeta_CircularArray_Cursor const* cursor_b = cursor_b_;

    CheckCursor_(ca, cursor_a);
    CheckCursor_(ca, cursor_b);

    return ZETA_ThreeWayCompare(cursor_a->idx + 1, cursor_b->idx + 1);
}

size_t Zeta_CircularArray_Cursor_GetDist(void const* ca_, void const* cursor_a_,
                                         void const* cursor_b_) {
    Zeta_CircularArray const* ca = ca_;
    Zeta_CircularArray_Cursor const* cursor_a = cursor_a_;
    Zeta_CircularArray_Cursor const* cursor_b = cursor_b_;

    CheckCursor_(ca, cursor_a);
    CheckCursor_(ca, cursor_b);

    return cursor_b->idx - cursor_a->idx;
}

size_t Zeta_CircularArray_Cursor_GetIdx(void const* ca_, void const* cursor_) {
    Zeta_CircularArray const* ca = ca_;
    Zeta_CircularArray_Cursor const* cursor = cursor_;

    CheckCursor_(ca, cursor);

    return cursor->idx;
}

void Zeta_CircularArray_Cursor_StepL(void const* ca, void* cursor) {
    Zeta_CircularArray_Cursor_AdvanceL(ca, cursor, 1);
}

void Zeta_CircularArray_Cursor_StepR(void const* ca, void* cursor) {
    Zeta_CircularArray_Cursor_AdvanceR(ca, cursor, 1);
}

void Zeta_CircularArray_Cursor_AdvanceL(void const* ca_, void* cursor_,
                                        size_t step) {
    Zeta_CircularArray const* ca = ca_;
    Zeta_CircularArray_Cursor* cursor = cursor_;

    CheckCursor_(ca, cursor);

    ZETA_DebugAssert(step <= cursor->idx + 1);

    size_t idx = cursor->idx - step;

    cursor->idx = idx;

    cursor->ref = idx < ca->size ? Refer_(ca->data, ca->width, ca->offset, idx,
                                          ca->capacity)
                                 : NULL;
}

void Zeta_CircularArray_Cursor_AdvanceR(void const* ca_, void* cursor_,
                                        size_t step) {
    Zeta_CircularArray const* ca = ca_;
    Zeta_CircularArray_Cursor* cursor = cursor_;

    CheckCursor_(ca, cursor);

    ZETA_DebugAssert(step <= ca->size - cursor->idx);

    size_t idx = cursor->idx + step;

    cursor->idx = idx;

    cursor->ref = idx < ca->size ? Refer_(ca->data, ca->width, ca->offset, idx,
                                          ca->capacity)
                                 : NULL;
}

void Zeta_CircularArray_Cursor_Check(void const* ca_, void const* cursor_) {
    Zeta_CircularArray const* ca = ca_;
    CheckCntr_(ca);

    Zeta_CircularArray_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ZETA_DebugAssert(ca == cursor->ca);
    ZETA_DebugAssert(cursor->idx + 1 < ca->size + 2);

    void* data = ca->data;
    size_t stride = ca->stride;
    size_t offset = ca->offset;
    size_t size = ca->size;
    size_t capacity = ca->capacity;

    if (cursor->idx < size) {
        ZETA_DebugAssert(cursor->ref ==
                         Refer_(data, stride, offset, cursor->idx, capacity));
    }
}

Zeta_SeqCntr_VTable const zeta_circular_array_seq_cntr_vtable = {
    .cursor_size = sizeof(Zeta_CircularArray_Cursor),

    .GetWidth = Zeta_CircularArray_GetWidth,

    .GetSize = Zeta_CircularArray_GetSize,

    .GetCapacity = Zeta_CircularArray_GetCapacity,

    .GetLBCursor = Zeta_CircularArray_GetLBCursor,

    .GetRBCursor = Zeta_CircularArray_GetRBCursor,

    .PeekL = Zeta_CircularArray_PeekL,

    .PeekR = Zeta_CircularArray_PeekR,

    .Access = Zeta_CircularArray_Access,

    .Refer = Zeta_CircularArray_Refer,

    .Read = Zeta_CircularArray_Read,

    .Write = Zeta_CircularArray_Write,

    .PushL = Zeta_CircularArray_PushL,

    .PushR = Zeta_CircularArray_PushR,

    .Insert = Zeta_CircularArray_Insert,

    .PopL = Zeta_CircularArray_PopL,

    .PopR = Zeta_CircularArray_PopR,

    .Erase = Zeta_CircularArray_Erase,

    .EraseAll = Zeta_CircularArray_EraseAll,

    .Cursor_AreEqual = Zeta_CircularArray_Cursor_AreEqual,

    .Cursor_Compare = Zeta_CircularArray_Cursor_Compare,

    .Cursor_GetDist = Zeta_CircularArray_Cursor_GetDist,

    .Cursor_GetIdx = Zeta_CircularArray_Cursor_GetIdx,

    .Cursor_StepL = Zeta_CircularArray_Cursor_StepL,

    .Cursor_StepR = Zeta_CircularArray_Cursor_StepR,

    .Cursor_AdvanceL = Zeta_CircularArray_Cursor_AdvanceL,

    .Cursor_AdvanceR = Zeta_CircularArray_Cursor_AdvanceR,
};
