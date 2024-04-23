#include "DebugDeque.h"

#include <deque>

void Zeta_DebugDeque_Create(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    dd->deque = new std::deque<void*>;

    ZETA_DebugAssert(0 < dd->width);
}

void Zeta_DebugDeque_Destroy(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    for (size_t i = 0, end = deque->size(); i < end; ++i) {
        delete[] (char*)(*deque)[i];
    }

    delete deque;
}

size_t Zeta_DebugDeque_GetWidth(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    return dd->width;
}

size_t Zeta_DebugDeque_GetSize(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    return deque->size();
}

void* Zeta_DebugDeque_PeekL(void* dd_, void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)(dst_cursor_);
    if (dst_cursor != NULL) { *dst_cursor = 0; }

    return deque->empty() ? NULL : deque->front();
}

void* Zeta_DebugDeque_PeekR(void* dd_, void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)(dst_cursor_);
    if (dst_cursor != NULL) { *dst_cursor = deque->size() - 1; }

    return deque->empty() ? NULL : deque->back();
}

void* Zeta_DebugDeque_Access(void* dd_, void* dst_cursor_, size_t idx) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(idx + 1 <= deque->size() + 1);

    size_t* dst_cursor = (size_t*)(dst_cursor_);

    if (dst_cursor != NULL) { *dst_cursor = idx; }

    return idx < deque->size() ? (*deque)[idx] : NULL;
}

void* Zeta_DebugDeque_PushL(void* dd_, void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)(dst_cursor_);
    if (dst_cursor != NULL) { *dst_cursor = 0; }

    deque->push_front(new unsigned char[dd->width]);

    return deque->front();
}

void* Zeta_DebugDeque_PushR(void* dd_, void* dst_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* dst_cursor = (size_t*)(dst_cursor_);
    if (dst_cursor != NULL) { *dst_cursor = 0; }

    deque->push_back(new unsigned char[dd->width]);

    return deque->front();
}

void* Zeta_DebugDeque_Insert(void* dd_, void* pos_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* pos_cursor = (size_t*)pos_cursor_;
    ZETA_DebugAssert(pos_cursor != NULL);

    size_t idx = *pos_cursor;
    ZETA_DebugAssert(idx <= deque->size());

    return *deque->insert(deque->begin() + idx, new unsigned char[dd->width]);
}

void Zeta_DebugDeque_PopL(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(0 < deque->size());

    delete[] (unsigned char*)deque->front();
    deque->pop_front();
}

void Zeta_DebugDeque_PopR(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(0 < deque->size());

    delete[] (unsigned char*)deque->back();
    deque->pop_back();
}

void* Zeta_DebugDeque_Erase(void* dd_, void* pos_cursor_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    size_t* pos_cursor = (size_t*)pos_cursor_;
    ZETA_DebugAssert(pos_cursor != NULL);

    size_t idx = *pos_cursor;
    ZETA_DebugAssert(idx < deque->size());

    delete[] (unsigned char*)(*deque)[idx];
    deque->erase(deque->begin() + idx);

    return idx < deque->size() ? (*deque)[idx] : NULL;
}

void Zeta_DebugDeque_EraseAll(void* dd_, void* callback_context,
                              void (*Callback)(void* context, void* ele)) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    if (Callback == NULL) {
        for (auto iter{ deque->begin() }, end{ deque->end() }; iter != end;
             ++iter) {
            delete[] (unsigned char*)(*iter);
        }
    } else {
        for (auto iter{ deque->begin() }, end{ deque->end() }; iter != end;
             ++iter) {
            Callback(callback_context, *iter);
            delete[] (unsigned char*)(*iter);
        }
    }

    deque->clear();
}

void Zeta_DebugDeque_DeploySeqContainer(void* dd_, Zeta_SeqContainer* dst) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = dd;

    dst->GetWidth = Zeta_DebugDeque_GetWidth;
    dst->GetSize = Zeta_DebugDeque_GetSize;

    dst->PeekL = Zeta_DebugDeque_PeekL;
    dst->PeekR = Zeta_DebugDeque_PeekR;

    dst->Access = Zeta_DebugDeque_Access;

    dst->PushL = Zeta_DebugDeque_PushL;
    dst->PushR = Zeta_DebugDeque_PushR;

    dst->Insert = Zeta_DebugDeque_Insert;

    dst->PopL = Zeta_DebugDeque_PopL;
    dst->PopR = Zeta_DebugDeque_PopR;

    dst->Erase = Zeta_DebugDeque_Erase;
    dst->EraseAll = Zeta_DebugDeque_EraseAll;
}
