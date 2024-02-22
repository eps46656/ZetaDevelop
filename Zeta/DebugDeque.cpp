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

void* Zeta_DebugDeque_PeekL(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(0 < deque->size());

    return Zeta_DebugDeque_Access(dd, 0);
}

void* Zeta_DebugDeque_PeekR(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(0 < deque->size());

    return Zeta_DebugDeque_Access(dd, deque->size() - 1);
}

void* Zeta_DebugDeque_Access(void* dd_, size_t idx) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(idx < deque->size());

    return (*deque)[idx];
}

void* Zeta_DebugDeque_PushL(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    return Zeta_DebugDeque_Insert(dd, 0);
}

void* Zeta_DebugDeque_PushR(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    return Zeta_DebugDeque_Insert(dd, deque->size());
}

void* Zeta_DebugDeque_Insert(void* dd_, size_t idx) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(idx <= deque->size());

    return *deque->insert(deque->begin() + idx, new char[dd->width]);
}

void Zeta_DebugDeque_PopL(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(0 < deque->size());

    Zeta_DebugDeque_Erase(dd, 0);
}

void Zeta_DebugDeque_PopR(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(0 < deque->size());

    Zeta_DebugDeque_Erase(dd, deque->size() - 1);
}

void Zeta_DebugDeque_Erase(void* dd_, size_t idx) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    std::deque<void*>* deque = (std::deque<void*>*)dd->deque;

    ZETA_DebugAssert(idx < deque->size());

    delete[] (char*)(*deque)[idx];

    deque->erase(deque->begin() + idx);
}

void Zeta_DebugDeque_ToVector(void* dd_, Zeta_Vector* dst) {
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
}
