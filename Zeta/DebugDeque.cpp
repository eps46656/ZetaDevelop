#include "DebugDeque.h"

#include <deque>

#include "Debugger.h"
#include "utils.h"

void Zeta_DebugDeque_Init(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    dd->deque = new std::deque<void*>;

    ZETA_DebugAssert(0 < dd->width);
    ZETA_DebugAssert(dd->width <= dd->stride);
}

void Zeta_DebugDeque_Deinit(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    for (size_t i = 0, end = deque->size(); i < end; ++i) {
        delete[] (unsigned char*)(*deque)[i];
    }

    delete deque;
}

size_t Zeta_DebugDeque_GetWidth(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    return dd->width;
}

size_t Zeta_DebugDeque_GetStride(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    return dd->stride;
}

size_t Zeta_DebugDeque_GetSize(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    return deque->size();
}

void Zeta_DebugDeque_GetLBCursor(void* dd_, void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    size_t* dst_cursor = (size_t*)dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    *dst_cursor = (size_t)(-1);
}

void Zeta_DebugDeque_GetRBCursor(void* dd_, void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)dst_cursor_;
    ZETA_DebugAssert(dst_cursor != NULL);

    *dst_cursor = deque->size();
}

void* Zeta_DebugDeque_PeekL(void* dd_, void* dst_cursor_, void* dst_ele) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)dst_cursor_;
    if (dst_cursor != NULL) { *dst_cursor = 0; }

    if (deque->empty()) { return NULL; }

    void* ele = deque->front();

    if (dst_ele != NULL) { Zeta_MemCopy(dst_ele, ele, dd->width); }

    return ele;
}

void* Zeta_DebugDeque_PeekR(void* dd_, void* dst_cursor_, void* dst_ele) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)dst_cursor_;
    if (dst_cursor != NULL) { *dst_cursor = deque->size() - 1; }

    if (deque->empty()) { return NULL; }

    void* ele = deque->back();

    if (dst_ele != NULL) { Zeta_MemCopy(dst_ele, ele, dd->width); }

    return ele;
}

void* Zeta_DebugDeque_Access(void* dd_, void* dst_cursor_, void* dst_ele,
                             size_t idx) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(idx + 1 < deque->size() + 2);

    size_t* dst_cursor = (size_t*)dst_cursor_;

    if (dst_cursor != NULL) { *dst_cursor = idx; }

    if (deque->size() <= idx) { return NULL; }

    void* ele = (*deque)[idx];

    if (dst_ele != NULL) { Zeta_MemCopy(dst_ele, ele, dd->width); }

    return ele;
}

void* Zeta_DebugDeque_Refer(void* dd_, void* pos_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    size_t* pos_cursor = (size_t*)pos_cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, pos_cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t idx = *pos_cursor;

    return idx < deque->size() ? (*deque)[idx] : NULL;
}

void Zeta_DebugDeque_Read(void* dd_, void* pos_cursor_, size_t cnt, void* dst_,
                          void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    size_t* pos_cursor = (size_t*)pos_cursor_;
    size_t* dst_cursor = (size_t*)dst_cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, pos_cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    unsigned char* dst = (unsigned char*)dst_;

    size_t beg = *pos_cursor;
    ZETA_DebugAssert(beg <= deque->size());

    ZETA_DebugAssert(cnt <= deque->size() - beg);

    size_t end = beg + cnt;

    if (dst_cursor != NULL) { *dst_cursor = end; }

    for (size_t idx = beg; idx < end; ++idx) {
        Zeta_MemCopy(dst, (*deque)[idx], dd->width);
        dst += dd->stride;
    }
}

void Zeta_DebugDeque_Write(void* dd_, void* pos_cursor_, size_t cnt,
                           void const* src_, void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    size_t* pos_cursor = (size_t*)pos_cursor_;
    size_t* dst_cursor = (size_t*)dst_cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, pos_cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    unsigned char const* src = (unsigned char const*)src_;

    size_t beg = *pos_cursor;
    ZETA_DebugAssert(beg <= deque->size());

    ZETA_DebugAssert(cnt <= deque->size() - beg);

    size_t end = beg + cnt;

    if (dst_cursor != NULL) { *dst_cursor = end; }

    for (size_t idx = beg; idx < end; ++idx, src += dd->stride) {
        Zeta_MemCopy((*deque)[idx], src, dd->width);
    }
}

void* Zeta_DebugDeque_PushL(void* dd_, void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)dst_cursor_;
    if (dst_cursor != NULL) { *dst_cursor = 0; }

    deque->push_front(new unsigned char[dd->width]);

    return deque->front();
}

void* Zeta_DebugDeque_PushR(void* dd_, void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)dst_cursor_;
    if (dst_cursor != NULL) { *dst_cursor = 0; }

    deque->push_back(new unsigned char[dd->width]);

    return deque->back();
}

void* Zeta_DebugDeque_Insert(void* dd_, void* pos_cursor_, size_t cnt) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    size_t* pos_cursor = (size_t*)pos_cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, pos_cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t idx = *pos_cursor;
    ZETA_DebugAssert(idx <= deque->size());

    deque->insert(deque->begin() + idx, cnt, NULL);

    size_t beg = idx;
    size_t end = beg + cnt;

    for (; idx < end; ++idx) { (*deque)[idx] = new unsigned char[dd->width]; }

    return beg < deque->size() ? (*deque)[beg] : NULL;
}

void Zeta_DebugDeque_PopL(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(0 < deque->size());

    delete[] (unsigned char*)deque->front();
    deque->pop_front();
}

void Zeta_DebugDeque_PopR(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(0 < deque->size());

    delete[] (unsigned char*)deque->back();
    deque->pop_back();
}

void Zeta_DebugDeque_Erase(void* dd_, void* pos_cursor_, size_t cnt) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    size_t* pos_cursor = (size_t*)pos_cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, pos_cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t beg = *pos_cursor;
    ZETA_DebugAssert(beg <= deque->size());

    ZETA_DebugAssert(cnt <= deque->size() - beg);

    size_t end = beg + cnt;

    for (size_t idx = beg; idx < end; ++idx) {
        delete[] (unsigned char*)((*deque)[idx]);
    }

    deque->erase(deque->begin() + beg, deque->begin() + end);
}

void Zeta_DebugDeque_EraseAll(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    for (auto iter{ deque->begin() }, end{ deque->end() }; iter != end;
         ++iter) {
        delete[] (unsigned char*)(*iter);
    }

    deque->clear();
}

void Zeta_DebugDeque_Check(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;
    ZETA_DebugAssert(deque != NULL);
}

bool_t Zeta_DebugDeque_Cursor_IsEqual(void* dd_, void const* cursor_a_,
                                      void const* cursor_b_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    size_t* cursor_a = (size_t*)cursor_a_;
    size_t* cursor_b = (size_t*)cursor_b_;

    Zeta_DebugDeque_Cursor_Check(dd, cursor_a);
    Zeta_DebugDeque_Cursor_Check(dd, cursor_b);

    ZETA_DebugAssert(cursor_a != NULL);
    ZETA_DebugAssert(cursor_b != NULL);

    return *cursor_a == *cursor_b;
}

int Zeta_DebugDeque_Cursor_Compare(void* dd_, void const* cursor_a_,
                                   void const* cursor_b_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;

    size_t* cursor_a = (size_t*)cursor_a_;
    size_t* cursor_b = (size_t*)cursor_b_;

    Zeta_DebugDeque_Cursor_Check(dd, cursor_a);
    Zeta_DebugDeque_Cursor_Check(dd, cursor_b);

    size_t ka = *cursor_a + 1;
    size_t kb = *cursor_b + 1;

    if (ka < kb) { return -1; }
    if (kb < ka) { return 1; }
    return 0;
}

size_t Zeta_DebugDeque_Cursor_GetDist(void* dd_, void const* cursor_a_,
                                      void const* cursor_b_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;

    size_t* cursor_a = (size_t*)cursor_a_;
    size_t* cursor_b = (size_t*)cursor_b_;

    Zeta_DebugDeque_Cursor_Check(dd, cursor_a);
    Zeta_DebugDeque_Cursor_Check(dd, cursor_b);

    return *cursor_b - *cursor_a;
}

size_t Zeta_DebugDeque_Cursor_GetIdx(void* dd_, void const* cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    size_t* cursor = (size_t*)cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, cursor);

    return *cursor;
}

void Zeta_DebugDeque_Cursor_StepL(void* dd, void* cursor) {
    Zeta_DebugDeque_Cursor_AdvanceL(dd, cursor, 1);
}

void Zeta_DebugDeque_Cursor_StepR(void* dd, void* cursor) {
    Zeta_DebugDeque_Cursor_AdvanceR(dd, cursor, 1);
}

void Zeta_DebugDeque_Cursor_AdvanceL(void* dd_, void* cursor_, size_t step) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    size_t* cursor = (size_t*)cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t idx = *cursor - step;

    ZETA_DebugAssert(idx + 1 < deque->size() + 2);

    *cursor = idx;
}

void Zeta_DebugDeque_Cursor_AdvanceR(void* dd_, void* cursor_, size_t step) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    size_t* cursor = (size_t*)cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t idx = *cursor + step;

    ZETA_DebugAssert(idx + 1 < deque->size() + 2);

    *cursor = idx;
}

void Zeta_DebugDeque_Cursor_Check(void* dd_, void const* cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;
    ZETA_DebugAssert(deque != NULL);

    size_t* cursor = (size_t*)cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ZETA_DebugAssert(*cursor + 1 < deque->size() + 2);
}

void Zeta_DebugDeque_DeploySeqContainer(void* dd_,
                                        Zeta_SeqContainer* seq_cntr) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    ZETA_DebugAssert(seq_cntr != NULL);

    seq_cntr->context = dd;

    seq_cntr->cursor_width = sizeof(size_t);

    seq_cntr->GetWidth = Zeta_DebugDeque_GetWidth;

    seq_cntr->GetStride = Zeta_DebugDeque_GetStride;

    seq_cntr->GetSize = Zeta_DebugDeque_GetSize;

    seq_cntr->GetLBCursor = Zeta_DebugDeque_GetLBCursor;

    seq_cntr->GetRBCursor = Zeta_DebugDeque_GetRBCursor;

    seq_cntr->PeekL = Zeta_DebugDeque_PeekL;

    seq_cntr->PeekR = Zeta_DebugDeque_PeekR;

    seq_cntr->Access = Zeta_DebugDeque_Access;

    seq_cntr->Refer = Zeta_DebugDeque_Refer;

    seq_cntr->Read = Zeta_DebugDeque_Read;

    seq_cntr->Write = Zeta_DebugDeque_Write;

    seq_cntr->PushL = Zeta_DebugDeque_PushL;

    seq_cntr->PushR = Zeta_DebugDeque_PushR;

    seq_cntr->Insert = Zeta_DebugDeque_Insert;

    seq_cntr->PopL = Zeta_DebugDeque_PopL;

    seq_cntr->PopR = Zeta_DebugDeque_PopR;

    seq_cntr->Erase = Zeta_DebugDeque_Erase;

    seq_cntr->EraseAll = Zeta_DebugDeque_EraseAll;

    seq_cntr->Cursor_IsEqual = Zeta_DebugDeque_Cursor_IsEqual;

    seq_cntr->Cursor_Compare = Zeta_DebugDeque_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_DebugDeque_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_DebugDeque_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_DebugDeque_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_DebugDeque_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_DebugDeque_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_DebugDeque_Cursor_AdvanceR;
}
