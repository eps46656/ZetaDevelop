#include "CircularVector.h"

#include "Debugger.h"
#include "utils.h"

#define Refer_(data, stride, offset, idx, capacity) \
    ((void*)((unsigned char*)(data) +               \
             (stride) * (((offset) + (idx)) % (capacity))))

void Zeta_CircularVector_Init(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    cv->data = NULL;
    cv->width = 0;
    cv->stride = 0;
    cv->offset = 0;
    cv->size = 0;
    cv->capacity = 0;
}

size_t Zeta_CircularVector_GetWidth(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    return cv->width;
}

size_t Zeta_CircularVector_GetStride(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    return cv->stride;
}

size_t Zeta_CircularVector_GetOffset(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    return cv->offset;
}

size_t Zeta_CircularVector_GetSize(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    return cv->size;
}

size_t Zeta_CircularVector_GetCapacity(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    return cv->capacity;
}

void Zeta_CircularVector_GetLBCursor(void* cv_, void* dst_cursor_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    dst_cursor->cv = cv;
    dst_cursor->idx = -1;
}

void Zeta_CircularVector_GetRBCursor(void* cv_, void* dst_cursor_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    dst_cursor->cv = cv;
    dst_cursor->idx = cv->size;
}

void* Zeta_CircularVector_PeekL(void* cv_, void* dst_cursor_, void* dst_ele) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;

    void* data = cv->data;
    size_t width = cv->width;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    void* ele = 0 < size ? Refer_(data, stride, offset, 0, capacity) : NULL;

    if (dst_cursor != NULL) {
        dst_cursor->cv = cv;
        dst_cursor->idx = 0;
        dst_cursor->ele = ele;
    }

    if (ele != NULL && dst_ele != NULL) { Zeta_MemCopy(dst_ele, ele, width); }

    return ele;
}

void* Zeta_CircularVector_PeekR(void* cv_, void* dst_cursor_, void* dst_ele) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;

    void* data = cv->data;
    size_t width = cv->width;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    void* ele =
        0 < size ? Refer_(data, stride, offset, size - 1, capacity) : NULL;

    if (dst_cursor != NULL) {
        dst_cursor->cv = cv;
        dst_cursor->idx = size - 1;
        dst_cursor->ele = ele;
    }

    if (ele != NULL && dst_ele != NULL) { Zeta_MemCopy(dst_ele, ele, width); }

    return ele;
}

void* Zeta_CircularVector_Access(void* cv_, void* dst_cursor_, void* dst_ele,
                                 size_t idx) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;

    void* data = cv->data;
    size_t width = cv->width;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    ZETA_DebugAssert(idx + 1 < size + 2);

    void* ele = idx < size ? Refer_(data, stride, offset, idx, capacity) : NULL;

    if (dst_cursor != NULL) {
        dst_cursor->cv = cv;
        dst_cursor->idx = idx;
        dst_cursor->ele = ele;
    }

    if (ele != NULL && dst_ele != NULL) { Zeta_MemCopy(dst_ele, ele, width); }

    return ele;
}

void* Zeta_CircularVector_Refer(void* cv_, void const* pos_cursor_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor const* pos_cursor = pos_cursor_;
    Zeta_CircularVector_Cursor_Check(cv, pos_cursor);

    return pos_cursor->ele;
}

void Zeta_CircularVector_Read(void* cv_, void const* pos_cursor_, size_t cnt,
                              void* dst_, void* dst_cursor_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor const* pos_cursor = pos_cursor_;
    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;
    Zeta_CircularVector_Cursor_Check(cv, pos_cursor);

    if (cnt == 0) { return; }

    unsigned char* dst = dst_;
    ZETA_DebugAssert(dst != NULL);

    void* data = cv->data;
    size_t width = cv->width;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    size_t idx = pos_cursor->idx;

    ZETA_DebugAssert(idx <= size);
    ZETA_DebugAssert(cnt <= size - idx);

    if (stride - width < ZETA_CircularVector_vec_mem_copy_th) {
        while (0 < cnt) {
            size_t cur_cnt = ZETA_GetMinOf(
                cnt, Zeta_CircularVector_GetLongestContSucr(cv, idx));
            cnt -= cur_cnt;

            Zeta_MemCopy(dst, Refer_(data, stride, offset, idx, capacity),
                         stride * cur_cnt);

            dst += stride * cur_cnt;
            idx += cur_cnt;
        }
    } else {
        for (size_t end = idx + cnt; idx < end; ++idx, dst += stride) {
            Zeta_MemCopy(dst, Refer_(data, stride, offset, idx, capacity),
                         width);
        }
    }

    if (dst_cursor != NULL) {
        dst_cursor->cv = cv;
        dst_cursor->idx = idx;
        dst_cursor->ele =
            idx == size ? NULL : Refer_(data, stride, offset, idx, capacity);
    }
}

void Zeta_CircularVector_Write(void* cv_, void* pos_cursor_, size_t cnt,
                               void const* src_, void* dst_cursor_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;
    Zeta_CircularVector_Cursor_Check(cv, pos_cursor);

    if (cnt == 0) {
        if (dst_cursor != NULL) {
            dst_cursor->cv = cv;
            dst_cursor->idx = pos_cursor->idx;
            dst_cursor->ele = pos_cursor->ele;
        }

        return;
    }

    unsigned char const* src = src_;
    ZETA_DebugAssert(src != NULL);

    void* data = cv->data;
    size_t width = cv->width;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    size_t idx = pos_cursor->idx;

    ZETA_DebugAssert(idx <= size);
    ZETA_DebugAssert(cnt <= size - idx);

    if (stride - width < ZETA_CircularVector_vec_mem_copy_th) {
        while (0 < cnt) {
            size_t cur_cnt = ZETA_GetMinOf(
                cnt, Zeta_CircularVector_GetLongestContSucr(cv, idx));
            cnt -= cur_cnt;

            Zeta_MemCopy(Refer_(data, stride, offset, idx, capacity), src,
                         stride * cur_cnt);

            idx += cur_cnt;
            src += stride * cur_cnt;
        }
    } else {
        for (size_t end = idx + cnt; idx < end; ++idx, src += stride) {
            Zeta_MemCopy(Refer_(data, stride, offset, idx, capacity), src,
                         width);
        }
    }

    if (dst_cursor != NULL) {
        dst_cursor->cv = cv;
        dst_cursor->idx = idx;
        dst_cursor->ele =
            idx == size ? NULL : Refer_(data, stride, offset, idx, capacity);
    }
}

void* Zeta_CircularVector_PushL(void* cv_, void* dst_cursor_, size_t cnt) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_CircularVector_Cursor cursor;
    cursor.cv = cv;
    cursor.idx = 0;

    Zeta_CircularVector_Insert(cv, &cursor, cnt);

    if (dst_cursor != NULL) {
        dst_cursor->cv = cursor.cv;
        dst_cursor->idx = cursor.idx;
        dst_cursor->ele = cursor.ele;
    }

    return cursor.ele;
}

void* Zeta_CircularVector_PushR(void* cv_, void* dst_cursor_, size_t cnt) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_CircularVector_Cursor cursor;
    cursor.cv = cv;
    cursor.idx = cv->size;

    Zeta_CircularVector_Insert(cv, &cursor, cnt);

    if (dst_cursor != NULL) {
        dst_cursor->cv = cursor.cv;
        dst_cursor->idx = cursor.idx;
        dst_cursor->ele = cursor.ele;
    }

    return cursor.ele;
}

void* Zeta_CircularVector_Insert(void* cv_, void* pos_cursor_, size_t cnt) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_CircularVector_Cursor_Check(cv, pos_cursor);

    void* data = cv->data;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    size_t idx = pos_cursor->idx;

    ZETA_DebugAssert(idx <= size);
    ZETA_DebugAssert(cnt <= capacity - size);

    size_t l_size = idx;
    size_t r_size = size - idx;

    cv->size = size += cnt;

    unsigned long long rand_seed =
        ZETA_PtrToAddr(data) + ZETA_PtrToAddr(pos_cursor) + idx + cnt;

    if (Zeta_Choose2(l_size <= r_size, r_size <= l_size, &rand_seed) == 0) {
        cv->offset = offset = (offset + capacity - cnt) % capacity;
        Zeta_CircularVector_Assign(cv, cv, 0, cnt, l_size);
    } else {
        Zeta_CircularVector_Assign(cv, cv, l_size + cnt, l_size, r_size);
    }

    pos_cursor->ele = Refer_(data, stride, offset, idx, capacity);

    return pos_cursor->ele;
}

void Zeta_CircularVector_PopL(void* cv_, size_t cnt) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    size_t size = cv->size;
    ZETA_DebugAssert(0 < size);

    Zeta_CircularVector_Cursor pos_cursor;
    pos_cursor.cv = cv;
    pos_cursor.idx = 0;

    Zeta_CircularVector_Erase(cv, &pos_cursor, cnt);
}

void Zeta_CircularVector_PopR(void* cv_, size_t cnt) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    size_t size = cv->size;
    ZETA_DebugAssert(0 < size);

    Zeta_CircularVector_Cursor pos_cursor;
    pos_cursor.cv = cv;
    pos_cursor.idx = size - 1;

    Zeta_CircularVector_Erase(cv, &pos_cursor, cnt);
}

void Zeta_CircularVector_Erase(void* cv_, void* pos_cursor_, size_t cnt) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor* pos_cursor = pos_cursor_;
    Zeta_CircularVector_Cursor_Check(cv, pos_cursor);

    void* data = cv->data;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    size_t idx = pos_cursor->idx;

    ZETA_DebugAssert(idx <= size);
    ZETA_DebugAssert(cnt <= size - idx);

    if (cnt == 0) { return; }

    size_t l_size = idx;
    size_t r_size = size - idx - cnt;

    unsigned long long rand_seed =
        ZETA_PtrToAddr(data) + ZETA_PtrToAddr(pos_cursor) + idx + cnt;

    if (Zeta_Choose2(l_size <= r_size, r_size <= l_size, &rand_seed)) {
        Zeta_CircularVector_Assign(cv, cv, cnt, 0, l_size);
        cv->offset = offset = (offset + cnt) % capacity;
    } else {
        Zeta_CircularVector_Assign(cv, cv, l_size, l_size + cnt, r_size);
    }

    cv->size = (size -= cnt);

    if (size == 0) { cv->offset = offset = 0; }

    if (idx < size) {
        pos_cursor->ele = Refer_(data, stride, offset, idx, capacity);
    }
}

void Zeta_CircularVector_EraseAll(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    cv->offset = 0;
    cv->size = 0;
}

void Zeta_CircularVector_Assign(void* dst_cv_, void* src_cv_, size_t dst_beg,
                                size_t src_beg, size_t cnt) {
    Zeta_CircularVector* dst_cv = dst_cv_;
    Zeta_CircularVector* src_cv = src_cv_;

    Zeta_CircularVector_Check(dst_cv);
    Zeta_CircularVector_Check(src_cv);

    void* dst_data = dst_cv->data;
    size_t dst_width = dst_cv->width;
    size_t dst_stride = dst_cv->stride;
    size_t dst_offset = dst_cv->offset;
    size_t dst_size = dst_cv->size;
    size_t dst_capacity = dst_cv->capacity;

    void* src_data = src_cv->data;
    size_t src_width = src_cv->width;
    size_t src_stride = src_cv->stride;
    size_t src_offset = src_cv->offset;
    size_t src_size = src_cv->size;
    size_t src_capacity = src_cv->capacity;

    ZETA_DebugAssert(dst_beg <= dst_size);
    ZETA_DebugAssert(src_beg <= src_size);

    if (cnt == 0) { return; }

    ZETA_DebugAssert(cnt <= dst_size - dst_beg);
    ZETA_DebugAssert(cnt <= src_size - src_beg);

    size_t width = ZETA_GetMinOf(dst_width, src_width);

    if (dst_stride != src_stride) { goto ELE_FW_COPY; }

    if (dst_stride - width < ZETA_CircularVector_vec_mem_copy_th) {
        if (dst_cv != src_cv) {
            ZETA_DebugAssert(!ZETA_AreOverlapped(dst_data, dst_capacity,
                                                 src_data, src_capacity));

            goto ELE_FW_COPY;
        }

        if (dst_beg == src_beg) { return; }

        if (dst_beg < src_beg) {
            goto ELE_FW_COPY;
        } else {
            goto ELE_BW_COPY;
        }
    } else {
        if (dst_cv != src_cv) {
            ZETA_DebugAssert(!ZETA_AreOverlapped(dst_data, dst_capacity,
                                                 src_data, src_capacity));

            goto VEC_MOVE;
        }

        if (dst_beg == src_beg) { return; }

        if (dst_beg < src_beg) {
            goto VEC_FW_COPY;
        } else {
            goto VEC_BW_COPY;
        }
    }

ELE_FW_COPY: {
    while (0 < cnt) {
        --cnt;

        Zeta_MemCopy(
            Refer_(dst_data, dst_stride, dst_offset, dst_beg, dst_capacity),
            Refer_(src_data, src_stride, src_offset, src_beg, src_capacity),
            dst_stride);

        ++dst_beg;
        ++src_beg;
    }

    return;
}

ELE_BW_COPY: {
    for (size_t dst_end = dst_beg + cnt, src_end = src_beg + cnt; 0 < cnt;) {
        --cnt;
        --dst_end;
        --src_end;

        Zeta_MemCopy(
            Refer_(dst_data, dst_stride, dst_offset, dst_end, dst_capacity),
            Refer_(src_data, src_stride, src_offset, src_end, src_capacity),
            width);
    }

    return;
}

VEC_MOVE: {
    while (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(
            cnt, ZETA_GetMinOf(
                     Zeta_CircularVector_GetLongestContSucr(dst_cv, dst_beg),
                     Zeta_CircularVector_GetLongestContSucr(src_cv, src_beg)));
        cnt -= cur_cnt;

        Zeta_MemMove(
            Refer_(dst_data, dst_stride, dst_offset, dst_beg, dst_capacity),
            Refer_(src_data, src_stride, src_offset, src_beg, src_capacity),
            dst_stride * cur_cnt);

        dst_beg += cur_cnt;
        src_beg += cur_cnt;
    }

    return;
}

VEC_FW_COPY: {
    while (0 < cnt) {
        size_t cur_cnt = ZETA_GetMinOf(
            cnt, ZETA_GetMinOf(
                     Zeta_CircularVector_GetLongestContSucr(dst_cv, dst_beg),
                     Zeta_CircularVector_GetLongestContSucr(src_cv, src_beg)));
        cnt -= cur_cnt;

        Zeta_MemCopy(
            Refer_(dst_data, dst_stride, dst_offset, dst_beg, dst_capacity),
            Refer_(src_data, src_stride, src_offset, src_beg, src_capacity),
            dst_stride * cur_cnt);

        dst_beg += cur_cnt;
        src_beg += cur_cnt;
    }

    return;
}

VEC_BW_COPY: {
    for (size_t dst_end = dst_beg + cnt, src_end = src_beg + cnt; 0 < cnt;) {
        size_t cur_cnt = ZETA_GetMinOf(
            cnt, ZETA_GetMinOf(
                     Zeta_CircularVector_GetLongestContPred(dst_cv, dst_end),
                     Zeta_CircularVector_GetLongestContPred(src_cv, src_end)));
        cnt -= cur_cnt;

        dst_end -= cur_cnt;
        src_end -= cur_cnt;

        Zeta_MemCopy(
            Refer_(dst_data, dst_stride, dst_offset, dst_end, dst_capacity),
            Refer_(src_data, src_stride, src_offset, src_end, src_capacity),
            dst_stride * cur_cnt);
    }

    return;
}
}

void Zeta_CircularVector_AssignFromSeqContainer(void* cv_, void* cv_cursor_,
                                                Zeta_SeqContainer* seq_cntr,
                                                void* seq_cntr_cursor,
                                                size_t cnt) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor* cv_cursor = cv_cursor_;
    Zeta_CircularVector_Cursor_Check(cv, cv_cursor);

    if (seq_cntr->Read == Zeta_CircularVector_Read) {
        Zeta_CircularVector_Cursor_Check(seq_cntr->context, seq_cntr_cursor);

        Zeta_CircularVector_Assign(
            cv, seq_cntr->context,
            ((Zeta_CircularVector_Cursor*)cv_cursor)->idx,
            ((Zeta_CircularVector_Cursor*)seq_cntr_cursor)->idx, cnt);

        return;
    }

    void* data = cv->data;
    size_t width = cv->width;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    size_t idx = ((Zeta_CircularVector_Cursor*)cv_cursor)->idx;

    ZETA_DebugAssert(idx <= size);
    ZETA_DebugAssert(cnt <= size - idx);

    ZETA_DebugAssert(seq_cntr->Read != NULL);

    ZETA_DebugLogCurPos;

    if (stride - width < ZETA_CircularVector_vec_mem_copy_th) {
        ZETA_DebugLogCurPos;

        while (0 < cnt) {
            ZETA_DebugLogCurPos;

            size_t cur_cnt = ZETA_GetMinOf(
                cnt, Zeta_CircularVector_GetLongestContSucr(cv, idx));
            cnt -= cur_cnt;

            ZETA_DebugLogCurPos;

            seq_cntr->Read(seq_cntr->context, seq_cntr_cursor, cur_cnt,
                           Refer_(data, stride, offset, idx, capacity),
                           seq_cntr_cursor);

            idx += cur_cnt;
        }
    } else {
        ZETA_DebugLogCurPos;

        for (size_t end = idx + cnt; idx < end; ++idx) {
            seq_cntr->Read(seq_cntr->context, seq_cntr_cursor, 1,
                           Refer_(data, stride, offset, idx, capacity),
                           seq_cntr_cursor);
        }
    }
}

void Zeta_CircularVector_Check(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    void* data = cv->data;
    size_t width = cv->width;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);
    ZETA_DebugAssert(offset == 0 || offset < capacity);
    ZETA_DebugAssert(size <= capacity);
    ZETA_DebugAssert(capacity <= ZETA_CircularVector_max_capacity);

    if (data == NULL) { ZETA_DebugAssert(capacity == 0); }
}

size_t Zeta_CircularVector_GetLongestContPred(void* cv_, size_t idx) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    ZETA_DebugAssert(idx <= size);

    size_t k = capacity - offset;

    return idx <= k ? idx : idx - k;
}

size_t Zeta_CircularVector_GetLongestContSucr(void* cv_, size_t idx) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    ZETA_DebugAssert(idx <= size);

    size_t k = capacity - offset;

    return (size <= k || k <= idx) ? size - idx : k - idx;
}

bool_t Zeta_CircularVector_Cursor_IsEqual(void* cv_, void const* cursor_a_,
                                          void const* cursor_b_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor const* cursor_a = cursor_a_;
    Zeta_CircularVector_Cursor const* cursor_b = cursor_b_;

    Zeta_CircularVector_Cursor_Check(cv, cursor_a);
    Zeta_CircularVector_Cursor_Check(cv, cursor_b);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_CircularVector_Cursor_Compare(void* cv_, void const* cursor_a_,
                                       void const* cursor_b_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor const* cursor_a = cursor_a_;
    Zeta_CircularVector_Cursor const* cursor_b = cursor_b_;

    Zeta_CircularVector_Cursor_Check(cv, cursor_a);
    Zeta_CircularVector_Cursor_Check(cv, cursor_b);

    size_t ka = cursor_a->idx + 1;
    size_t kb = cursor_b->idx + 1;

    if (ka < kb) { return -1; }
    if (kb < ka) { return 1; }
    return 0;
}

size_t Zeta_CircularVector_Cursor_GetDist(void* cv_, void const* cursor_a_,
                                          void const* cursor_b_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor const* cursor_a = cursor_a_;
    Zeta_CircularVector_Cursor const* cursor_b = cursor_b_;

    Zeta_CircularVector_Cursor_Check(cv, cursor_a);
    Zeta_CircularVector_Cursor_Check(cv, cursor_b);

    return cursor_a->idx - cursor_b->idx;
}

size_t Zeta_CircularVector_Cursor_GetIdx(void* cv_, void const* cursor_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor const* cursor = cursor_;
    Zeta_CircularVector_Cursor_Check(cv, cursor);

    return cursor->idx;
}

void Zeta_CircularVector_Cursor_StepL(void* cv, void* cursor) {
    Zeta_CircularVector_Cursor_AdvanceL(cv, cursor, 1);
}

void Zeta_CircularVector_Cursor_StepR(void* cv, void* cursor) {
    Zeta_CircularVector_Cursor_AdvanceR(cv, cursor, 1);
}

void Zeta_CircularVector_Cursor_AdvanceL(void* cv_, void* cursor_,
                                         size_t step) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor* cursor = cursor_;
    Zeta_CircularVector_Cursor_Check(cv, cursor);

    size_t idx = cursor->idx - step;
    ZETA_DebugAssert(idx + 1 < cv->size + 2);

    cursor->idx = idx;

    cursor->ele = idx < cv->size ? Refer_(cv->data, cv->stride, cv->offset, idx,
                                          cv->capacity)
                                 : NULL;
}

void Zeta_CircularVector_Cursor_AdvanceR(void* cv_, void* cursor_,
                                         size_t step) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Cursor* cursor = cursor_;
    Zeta_CircularVector_Cursor_Check(cv, cursor);

    size_t idx = cursor->idx + step;
    ZETA_DebugAssert(idx + 1 < cv->size + 2);

    cursor->idx = idx;

    cursor->ele = idx < cv->size ? Refer_(cv->data, cv->stride, cv->offset, idx,
                                          cv->capacity)
                                 : NULL;
}

void Zeta_CircularVector_Cursor_Check(void* cv_, void const* cursor_) {
    Zeta_CircularVector* cv = cv_;
    Zeta_CircularVector_Check(cv);

    Zeta_CircularVector_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ZETA_DebugAssert(cv == cursor->cv);
    ZETA_DebugAssert(cursor->idx + 1 < cv->size + 2);

    void* data = cv->data;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;

    if (cursor->idx < size) {
        ZETA_DebugAssert(cursor->ele ==
                         Refer_(data, stride, offset, cursor->idx, capacity));
    }
}

void Zeta_CircularVector_DeploySeqContainer(void* cv_,
                                            Zeta_SeqContainer* seq_cntr) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    ZETA_DebugAssert(seq_cntr != NULL);

    Zeta_SeqContainer_Init(seq_cntr);

    seq_cntr->context = cv;

    seq_cntr->cursor_align = alignof(Zeta_CircularVector_Cursor);

    seq_cntr->cursor_size = sizeof(Zeta_CircularVector_Cursor);

    seq_cntr->GetStride = Zeta_CircularVector_GetStride;

    seq_cntr->GetWidth = Zeta_CircularVector_GetWidth;

    seq_cntr->GetSize = Zeta_CircularVector_GetSize;

    seq_cntr->GetCapacity = Zeta_CircularVector_GetCapacity;

    seq_cntr->GetLBCursor = Zeta_CircularVector_GetLBCursor;

    seq_cntr->GetRBCursor = Zeta_CircularVector_GetRBCursor;

    seq_cntr->PeekL = Zeta_CircularVector_PeekL;

    seq_cntr->PeekR = Zeta_CircularVector_PeekR;

    seq_cntr->Access = Zeta_CircularVector_Access;

    seq_cntr->Refer = Zeta_CircularVector_Refer;

    seq_cntr->Read = Zeta_CircularVector_Read;

    seq_cntr->Write = Zeta_CircularVector_Write;

    seq_cntr->PushL = Zeta_CircularVector_PushL;

    seq_cntr->PushR = Zeta_CircularVector_PushR;

    seq_cntr->Insert = Zeta_CircularVector_Insert;

    seq_cntr->PopL = Zeta_CircularVector_PopL;

    seq_cntr->PopR = Zeta_CircularVector_PopR;

    seq_cntr->Erase = Zeta_CircularVector_Erase;

    seq_cntr->EraseAll = Zeta_CircularVector_EraseAll;

    seq_cntr->Cursor_IsEqual = Zeta_CircularVector_Cursor_IsEqual;

    seq_cntr->Cursor_Compare = Zeta_CircularVector_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_CircularVector_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_CircularVector_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_CircularVector_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_CircularVector_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_CircularVector_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_CircularVector_Cursor_AdvanceR;
}
