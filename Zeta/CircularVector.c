#include "CircularVector.h"

#include "utils.h"

#define Access_(data, stride, offset, idx, capacity) \
    (void*)((unsigned char*)(data) + stride * ((offset + idx) % capacity))

void Zeta_CircularVector_Init(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    cv->width = 0;
    cv->stride = 0;
    cv->offset = 0;
    cv->size = 0;
    cv->capacity = 0;
    cv->data = NULL;
}

size_t Zeta_CircularVector_GetWidth(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    return cv->width;
}

size_t Zeta_CircularVector_GetStride(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    return cv->stride;
}

size_t Zeta_CircularVector_GetOffset(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    return cv->offset;
}

size_t Zeta_CircularVector_GetSize(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    return cv->size;
}

size_t Zeta_CircularVector_GetCapacity(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    return cv->capacity;
}

void Zeta_CircularVector_GetLBCursor(void* cv_, void* dst_cursor_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    dst_cursor->idx = -1;
}

void Zeta_CircularVector_GetRBCursor(void* cv_, void* dst_cursor_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    dst_cursor->idx = cv->size;
}

void* Zeta_CircularVector_PeekL(void* cv_, void* dst_cursor) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    return Zeta_CircularVector_Access(cv, dst_cursor, 0);
}

void* Zeta_CircularVector_PeekR(void* cv_, void* dst_cursor) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    return Zeta_CircularVector_Access(cv, dst_cursor, cv->size - 1);
}

void* Zeta_CircularVector_Access(void* cv_, void* dst_cursor_, size_t idx) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;

    size_t width = cv->width;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;
    void* data = cv->data;

    ZETA_DebugAssert(width <= stride);
    ZETA_DebugAssert(offset < capacity);
    ZETA_DebugAssert(size <= capacity);
    ZETA_DebugAssert(capacity <= ZETA_GetMaxMod(size_t));
    ZETA_DebugAssert(data != NULL);

    ZETA_DebugAssert(idx + 1 < cv->size + 2);

    if (dst_cursor != NULL) { dst_cursor->idx = idx; }

    return idx < cv->size ? Access_(data, stride, offset, idx, capacity) : NULL;
}

void* Zeta_CircularVector_PushL(void* cv_, void* dst_cursor_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_CircularVector_Cursor c;
    c.idx = 0;

    void* ele = Zeta_CircularVector_Insert(cv, &c);

    if (dst_cursor != NULL) { dst_cursor->idx = c.idx; }

    return ele;
}

void* Zeta_CircularVector_PushR(void* cv_, void* dst_cursor_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor* dst_cursor = dst_cursor_;

    Zeta_CircularVector_Cursor c;
    c.idx = cv->size;

    void* ele = Zeta_CircularVector_Insert(cv, &c);

    if (dst_cursor != NULL) { dst_cursor->idx = c.idx; }

    return ele;
}

void* Zeta_CircularVector_Insert(void* cv_, void* pos_cursor_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor* pos_cursor = pos_cursor_;
    ZETA_DebugAssert(pos_cursor != NULL);

    size_t width = cv->width;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;
    void* data = cv->data;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);
    ZETA_DebugAssert(offset < capacity);
    ZETA_DebugAssert(size < capacity);
    ZETA_DebugAssert(capacity <= ZETA_GetMaxMod(size_t));
    ZETA_DebugAssert(data != NULL);

    size_t idx = pos_cursor->idx;
    ZETA_DebugAssert(idx <= size);

    size_t hole_idx;

    if (idx < size - idx) {
        hole_idx = 0;
        cv->offset = offset = (offset + capacity - 1) % capacity;
    } else {
        hole_idx = size;
    }

    cv->size = ++size;

    for (; hole_idx < idx; ++hole_idx) {
        Zeta_MemCopy(Access_(data, stride, offset, hole_idx, capacity),
                     Access_(data, stride, offset, hole_idx + 1, capacity),
                     width);
    }

    for (; idx < hole_idx; --hole_idx) {
        Zeta_MemCopy(Access_(data, stride, offset, hole_idx, capacity),
                     Access_(data, stride, offset, hole_idx - 1, capacity),
                     width);
    }

    return Access_(data, stride, offset, idx, capacity);
}

void Zeta_CircularVector_PopL(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor pos_cursor;
    pos_cursor.idx = 0;

    Zeta_CircularVector_Erase(cv, &pos_cursor);
}

void Zeta_CircularVector_PopR(void* cv_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor pos_cursor;
    pos_cursor.idx = cv->size - 1;

    Zeta_CircularVector_Erase(cv, &pos_cursor);
}

void* Zeta_CircularVector_Erase(void* cv_, void* pos_cursor_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor* pos_cursor = pos_cursor_;
    ZETA_DebugAssert(pos_cursor != NULL);

    size_t width = cv->width;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;
    void* data = cv->data;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);
    ZETA_DebugAssert(offset < capacity);
    ZETA_DebugAssert(size <= capacity);
    ZETA_DebugAssert(capacity <= ZETA_GetMaxMod(size_t));
    ZETA_DebugAssert(data != NULL);

    size_t idx = pos_cursor->idx;
    ZETA_DebugAssert(idx < size);

    if (idx < size - 1 - idx) {
        for (size_t i = idx; 0 < i; --i) {
            Zeta_MemCopy(Access_(data, stride, offset, i, capacity),
                         Access_(data, stride, offset, i - 1, capacity), width);
        }

        cv->offset = offset = (offset + 1) % capacity;
    } else {
        for (size_t i = idx; i < size - 1; ++i) {
            Zeta_MemCopy(Access_(data, stride, offset, i, capacity),
                         Access_(data, stride, offset, i + 1, capacity), width);
        }
    }

    cv->size = --size;

    return Access_(data, stride, offset, idx, capacity);
}

void Zeta_CircularVector_EraseAll(void* cv_, void* callback_context,
                                  void (*Callback)(void* context, void* ele)) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    if (Callback != NULL) {
        size_t stride = cv->stride;
        size_t offset = cv->offset;
        size_t size = cv->size;
        size_t capacity = cv->capacity;
        void* data = cv->data;

        for (size_t i = 0; i < size; ++i) {
            Callback(callback_context,
                     Access_(data, stride, offset, i, capacity));
        }
    }

    cv->offset = 0;
    cv->size = 0;
}

void Zeta_CircularVector_DeploySeqContainer(void* cv_,
                                            Zeta_SeqContainer* seq_cntr) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    ZETA_DebugAssert(seq_cntr != NULL);

    seq_cntr->context = cv;

    seq_cntr->GetWidth = Zeta_CircularVector_GetWidth;
    seq_cntr->GetSize = Zeta_CircularVector_GetSize;

    seq_cntr->GetLBCursor = Zeta_CircularVector_GetLBCursor;
    seq_cntr->GetRBCursor = Zeta_CircularVector_GetRBCursor;

    seq_cntr->PeekL = Zeta_CircularVector_PeekL;
    seq_cntr->PeekR = Zeta_CircularVector_PeekR;

    seq_cntr->Access = Zeta_CircularVector_Access;

    seq_cntr->PushL = Zeta_CircularVector_PushL;
    seq_cntr->PushR = Zeta_CircularVector_PushR;

    seq_cntr->Insert = Zeta_CircularVector_Insert;

    seq_cntr->PopL = Zeta_CircularVector_PopL;
    seq_cntr->PopR = Zeta_CircularVector_PopR;

    seq_cntr->Erase = Zeta_CircularVector_Erase;
    seq_cntr->EraseAll = Zeta_CircularVector_EraseAll;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void Zeta_CircularVector_Cursor_Check(void* cv_, void const* cursor_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    Zeta_CircularVector_Cursor re_cursor;
    Zeta_CircularVector_Access(cv, &re_cursor, cursor->idx);

    ZETA_DebugAssert(re_cursor.idx == cursor->idx);
}

bool_t Zeta_CircularVector_Cursor_IsEqual(void* cv_, void const* cursor_a_,
                                          void const* cursor_b_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor const* cursor_a = cursor_a_;
    Zeta_CircularVector_Cursor const* cursor_b = cursor_b_;

    ZETA_DebugAssert(cursor_a != NULL);
    ZETA_DebugAssert(cursor_b != NULL);

    return cursor_a->idx == cursor_b->idx;
}

int Zeta_CircularVector_Cursor_Compare(void* cv_, void const* cursor_a_,
                                       void const* cursor_b_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor const* cursor_a = cursor_a_;
    Zeta_CircularVector_Cursor const* cursor_b = cursor_b_;

    ZETA_DebugAssert(cursor_a != NULL);
    ZETA_DebugAssert(cursor_b != NULL);

    if (cursor_a->idx < cursor_b->idx) { return -1; }
    if (cursor_b->idx < cursor_a->idx) { return 1; }
    return 0;
}

size_t Zeta_CircularVector_Cursor_Differ(void* cv_, void const* cursor_a_,
                                         void const* cursor_b_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor const* cursor_a = cursor_a_;
    Zeta_CircularVector_Cursor const* cursor_b = cursor_b_;

    ZETA_DebugAssert(cursor_a != NULL);
    ZETA_DebugAssert(cursor_b != NULL);

    return cursor_a->idx - cursor_b->idx;
}

void* Zeta_CircularVector_Cursor_Refer(void* cv_, void const* cursor_) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor const* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    size_t width = cv->width;
    size_t stride = cv->stride;
    size_t offset = cv->offset;
    size_t size = cv->size;
    size_t capacity = cv->capacity;
    void* data = cv->data;

    ZETA_DebugAssert(0 < width);
    ZETA_DebugAssert(width <= stride);
    ZETA_DebugAssert(offset < capacity);
    ZETA_DebugAssert(size <= capacity);
    ZETA_DebugAssert(capacity <= ZETA_GetMaxMod(size_t));
    ZETA_DebugAssert(data != NULL);

    size_t idx = cursor->idx + 1;
    ZETA_DebugAssert(idx < size + 2);

    return (idx == 0 || idx == size + 1)
               ? NULL
               : Access_(data, stride, offset, idx - 1, capacity);
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
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    size_t size = cv->size;

    size_t idx = cursor->idx - step + 1;
    ZETA_DebugAssert(idx < size + 2);

    cursor->idx = idx - 1;
}

void Zeta_CircularVector_Cursor_AdvanceR(void* cv_, void* cursor_,
                                         size_t step) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    Zeta_CircularVector_Cursor* cursor = cursor_;
    ZETA_DebugAssert(cursor != NULL);

    size_t size = cv->size;

    size_t idx = cursor->idx + step + 1;
    ZETA_DebugAssert(idx < size + 2);

    cursor->idx = idx - 1;
}

void Zeta_CircularVector_Cursor_DeployCursorOperator(
    void* cv_, Zeta_CursorOperator* cursor_opr) {
    Zeta_CircularVector* cv = cv_;
    ZETA_DebugAssert(cv != NULL);

    ZETA_DebugAssert(cursor_opr != NULL);

    cursor_opr->context = cv;

    cursor_opr->cursor_width = sizeof(Zeta_CircularVector_Cursor);

    cursor_opr->IsEqual = Zeta_CircularVector_Cursor_IsEqual;
    cursor_opr->Compare = Zeta_CircularVector_Cursor_Compare;
    cursor_opr->Differ = Zeta_CircularVector_Cursor_Differ;

    cursor_opr->Refer = Zeta_CircularVector_Cursor_Refer;

    cursor_opr->StepL = Zeta_CircularVector_Cursor_StepL;
    cursor_opr->StepR = Zeta_CircularVector_Cursor_StepR;

    cursor_opr->AdvanceL = Zeta_CircularVector_Cursor_AdvanceL;
    cursor_opr->AdvanceR = Zeta_CircularVector_Cursor_AdvanceR;
}
