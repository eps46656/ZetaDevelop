#include "debug_deque.h"

#include <deque>

#include "debugger.h"
#include "utils.h"

extern "C" void Zeta_DebugDeque_Init(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    dd->deque = new std::deque<void*>;

    ZETA_DebugAssert(0 < dd->width);
}

extern "C" void Zeta_DebugDeque_Deinit(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    for (auto iter{ deque->begin() }, end{ deque->end() }; iter != end;
         ++iter) {
        delete[] (unsigned char*)(*iter);
    }

    delete deque;
}

extern "C" size_t Zeta_DebugDeque_GetWidth(void const* dd_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    Zeta_DebugDeque_Check(dd);

    return dd->width;
}

extern "C" size_t Zeta_DebugDeque_GetSize(void const* dd_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    return deque->size();
}

extern "C" size_t Zeta_DebugDeque_GetCapacity(void const* dd_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    return deque->max_size();
}

extern "C" void Zeta_DebugDeque_GetLBCursor(void const* dd_,
                                            void* dst_cursor_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    Zeta_DebugDeque_Check(dd);

    size_t* dst_cursor = (size_t*)dst_cursor_;

    if (dst_cursor == NULL) { return; }

    *dst_cursor = (size_t)(-1);
}

extern "C" void Zeta_DebugDeque_GetRBCursor(void const* dd_,
                                            void* dst_cursor_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)dst_cursor_;

    if (dst_cursor == NULL) { return; }

    *dst_cursor = deque->size();
}

extern "C" void* Zeta_DebugDeque_PeekL(void* dd_, void* dst_cursor_,
                                       void* dst_elem) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)dst_cursor_;
    if (dst_cursor != NULL) { *dst_cursor = 0; }

    if (deque->empty()) { return NULL; }

    void* elem = deque->front();

    if (dst_elem != NULL) { Zeta_MemCopy(dst_elem, elem, dd->width); }

    return elem;
}

extern "C" void* Zeta_DebugDeque_PeekR(void* dd_, void* dst_cursor_,
                                       void* dst_elem) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)dst_cursor_;
    if (dst_cursor != NULL) { *dst_cursor = deque->size() - 1; }

    if (deque->empty()) { return NULL; }

    void* elem = deque->back();

    if (dst_elem != NULL) { Zeta_MemCopy(dst_elem, elem, dd->width); }

    return elem;
}

extern "C" void* Zeta_DebugDeque_Access(void* dd_, size_t idx,
                                        void* dst_cursor_, void* dst_elem) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(idx + 1 < deque->size() + 2);

    size_t* dst_cursor = (size_t*)dst_cursor_;

    if (dst_cursor != NULL) { *dst_cursor = idx; }

    if (deque->size() <= idx) { return NULL; }

    void* elem = (*deque)[idx];

    if (dst_elem != NULL) { Zeta_MemCopy(dst_elem, elem, dd->width); }

    return elem;
}

extern "C" void* Zeta_DebugDeque_Refer(void* dd_, void const* pos_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    size_t const* pos_cursor = (size_t const*)pos_cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, pos_cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t idx = *pos_cursor;

    return idx < deque->size() ? (*deque)[idx] : NULL;
}

extern "C" void Zeta_DebugDeque_Read(void const* dd_, void const* pos_cursor_,
                                     size_t cnt, void* dst_, size_t dst_stride,
                                     void* dst_cursor_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    size_t const* pos_cursor = (size_t const*)pos_cursor_;
    size_t* dst_cursor = (size_t*)dst_cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, pos_cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    unsigned char* dst = (unsigned char*)dst_;
    ZETA_DebugAssert(dst != NULL);

    size_t beg = *pos_cursor;
    ZETA_DebugAssert(beg <= deque->size());

    ZETA_DebugAssert(cnt <= deque->size() - beg);

    size_t end = beg + cnt;

    if (dst_cursor != NULL) { *dst_cursor = end; }

    size_t dst_width = ZETA_GetMinOf(dst_stride, dd->width);

    for (size_t idx = beg; idx < end; ++idx) {
        Zeta_MemCopy(dst, (*deque)[idx], dst_width);
        dst += dst_stride;
    }
}

extern "C" void Zeta_DebugDeque_Write(void* dd_, void* pos_cursor_, size_t cnt,
                                      void const* src_, size_t src_stride,
                                      void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    size_t* pos_cursor = (size_t*)pos_cursor_;
    size_t* dst_cursor = (size_t*)dst_cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, pos_cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    unsigned char const* src = (unsigned char*)src_;
    ZETA_DebugAssert(src != NULL);

    size_t beg = *pos_cursor;
    ZETA_DebugAssert(beg <= deque->size());

    ZETA_DebugAssert(cnt <= deque->size() - beg);

    size_t end = beg + cnt;

    if (dst_cursor != NULL) { *dst_cursor = end; }

    size_t src_width = ZETA_GetMinOf(src_stride, dd->width);

    for (size_t idx = beg; idx < end; ++idx) {
        Zeta_MemCopy((*deque)[idx], src, src_width);
        src += src_stride;
    }
}

extern "C" void* Zeta_DebugDeque_PushL(void* dd_, size_t cnt,
                                       void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)dst_cursor_;
    if (dst_cursor != NULL) { *dst_cursor = 0; }

    deque->insert(deque->begin(), cnt, nullptr);

    for (size_t idx{ 0 }; idx < cnt; ++idx) {
        (*deque)[idx] = new unsigned char[dd->width];
    }

    return deque->empty() ? NULL : deque->front();
}

extern "C" void* Zeta_DebugDeque_PushR(void* dd_, size_t cnt,
                                       void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t origin_size{ deque->size() };

    size_t* dst_cursor = (size_t*)dst_cursor_;
    if (dst_cursor != NULL) { *dst_cursor = origin_size; }

    deque->insert(deque->end(), cnt, nullptr);

    for (size_t idx{ deque->size() - cnt }; idx < deque->size(); ++idx) {
        (*deque)[idx] = new unsigned char[dd->width];
    }

    return origin_size < deque->size() ? (*deque)[origin_size] : NULL;
}

extern "C" void* Zeta_DebugDeque_Insert(void* dd_, void* pos_cursor_,
                                        size_t cnt) {
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

extern "C" void Zeta_DebugDeque_PopL(void* dd_, size_t cnt) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(cnt <= deque->size());

    while (0 < cnt--) {
        delete[] (unsigned char*)deque->front();
        deque->pop_front();
    }
}

extern "C" void Zeta_DebugDeque_PopR(void* dd_, size_t cnt) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(cnt <= deque->size());

    while (0 < cnt--) {
        delete[] (unsigned char*)deque->back();
        deque->pop_back();
    }
}

extern "C" void Zeta_DebugDeque_Erase(void* dd_, void* pos_cursor_,
                                      size_t cnt) {
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

extern "C" void Zeta_DebugDeque_EraseAll(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    for (auto iter{ deque->begin() }, end{ deque->end() }; iter != end;
         ++iter) {
        delete[] (unsigned char*)(*iter);
    }

    deque->clear();
}

extern "C" void Zeta_DebugDeque_Check(void const* dd_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;
    ZETA_DebugAssert(deque != NULL);
}

extern "C" bool_t Zeta_DebugDeque_Cursor_AreEqual(void const* dd_,
                                                  void const* cursor_a_,
                                                  void const* cursor_b_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    size_t* cursor_a = (size_t*)cursor_a_;
    size_t* cursor_b = (size_t*)cursor_b_;

    Zeta_DebugDeque_Cursor_Check(dd, cursor_a);
    Zeta_DebugDeque_Cursor_Check(dd, cursor_b);

    ZETA_DebugAssert(cursor_a != NULL);
    ZETA_DebugAssert(cursor_b != NULL);

    return *cursor_a == *cursor_b;
}

extern "C" int Zeta_DebugDeque_Cursor_Compare(void const* dd_,
                                              void const* cursor_a_,
                                              void const* cursor_b_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;

    size_t* cursor_a = (size_t*)cursor_a_;
    size_t* cursor_b = (size_t*)cursor_b_;

    Zeta_DebugDeque_Cursor_Check(dd, cursor_a);
    Zeta_DebugDeque_Cursor_Check(dd, cursor_b);

    return ZETA_ThreeWayCompare(*cursor_a + 1, *cursor_b + 1);
}

extern "C" size_t Zeta_DebugDeque_Cursor_GetDist(void const* dd_,
                                                 void const* cursor_a_,
                                                 void const* cursor_b_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;

    size_t* cursor_a = (size_t*)cursor_a_;
    size_t* cursor_b = (size_t*)cursor_b_;

    Zeta_DebugDeque_Cursor_Check(dd, cursor_a);
    Zeta_DebugDeque_Cursor_Check(dd, cursor_b);

    return *cursor_b - *cursor_a;
}

extern "C" size_t Zeta_DebugDeque_Cursor_GetIdx(void const* dd_,
                                                void const* cursor_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    size_t* cursor = (size_t*)cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, cursor);

    return *cursor;
}

extern "C" void Zeta_DebugDeque_Cursor_StepL(void const* dd, void* cursor) {
    Zeta_DebugDeque_Cursor_AdvanceL(dd, cursor, 1);
}

extern "C" void Zeta_DebugDeque_Cursor_StepR(void const* dd, void* cursor) {
    Zeta_DebugDeque_Cursor_AdvanceR(dd, cursor, 1);
}

extern "C" void Zeta_DebugDeque_Cursor_AdvanceL(void const* dd_, void* cursor_,
                                                size_t step) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    size_t* cursor = (size_t*)cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t idx = *cursor - step;

    ZETA_DebugAssert(idx + 1 < deque->size() + 2);

    *cursor = idx;
}

extern "C" void Zeta_DebugDeque_Cursor_AdvanceR(void const* dd_, void* cursor_,
                                                size_t step) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    size_t* cursor = (size_t*)cursor_;
    Zeta_DebugDeque_Cursor_Check(dd, cursor);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t idx = *cursor + step;

    ZETA_DebugAssert(idx + 1 < deque->size() + 2);

    *cursor = idx;
}

extern "C" void Zeta_DebugDeque_Cursor_Check(void const* dd_,
                                             void const* cursor_) {
    Zeta_DebugDeque const* dd = (Zeta_DebugDeque const*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;
    ZETA_DebugAssert(deque != NULL);

    size_t* cursor = (size_t*)cursor_;
    ZETA_DebugAssert(cursor != NULL);

    ZETA_DebugAssert(*cursor + 1 < deque->size() + 2);
}

extern "C" void Zeta_DebugDeque_DeploySeqCntr(void* dd_,
                                              Zeta_SeqCntr* seq_cntr) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    Zeta_DebugDeque_Check(dd);

    ZETA_DebugAssert(seq_cntr != NULL);

    Zeta_SeqCntr_Init(seq_cntr);

    seq_cntr->context = dd;

    seq_cntr->const_context = dd;

    seq_cntr->cursor_size = sizeof(size_t);

    seq_cntr->GetWidth = Zeta_DebugDeque_GetWidth;

    seq_cntr->GetSize = Zeta_DebugDeque_GetSize;

    seq_cntr->GetCapacity = Zeta_DebugDeque_GetCapacity;

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

    seq_cntr->Cursor_AreEqual = Zeta_DebugDeque_Cursor_AreEqual;

    seq_cntr->Cursor_Compare = Zeta_DebugDeque_Cursor_Compare;

    seq_cntr->Cursor_GetDist = Zeta_DebugDeque_Cursor_GetDist;

    seq_cntr->Cursor_GetIdx = Zeta_DebugDeque_Cursor_GetIdx;

    seq_cntr->Cursor_StepL = Zeta_DebugDeque_Cursor_StepL;

    seq_cntr->Cursor_StepR = Zeta_DebugDeque_Cursor_StepR;

    seq_cntr->Cursor_AdvanceL = Zeta_DebugDeque_Cursor_AdvanceL;

    seq_cntr->Cursor_AdvanceR = Zeta_DebugDeque_Cursor_AdvanceR;
}
