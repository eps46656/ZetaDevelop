#include "DebugDeque.h"

#include <deque>

struct Zeta_DebugDeque {
    std::deque<Zeta_DebugDeque_val_t> deque;
};

void* Zeta_DebugDeque_Create() { return new std::deque<Zeta_DebugDeque_val_t>; }

void Zeta_DebugDeque_Destroy(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    delete dd;
}

size_t Zeta_DebugDeque_GetSize(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    return dd->deque.size();
}

Zeta_DebugDeque_val_t* Zeta_DebugDeque_Access(void* dd_, size_t idx) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    ZETA_DebugAssert(idx < dd->deque.size());

    return &dd->deque[idx];
}

Zeta_DebugDeque_val_t* Zeta_DebugDeque_Insert(void* dd_, size_t idx) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    ZETA_DebugAssert(idx <= dd->deque.size());

    auto iter{ dd->deque.insert(dd->deque.begin() + idx, NULL) };

    return &*iter;
}

Zeta_DebugDeque_val_t* Zeta_DebugDeque_PushL(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    return Zeta_DebugDeque_Insert(dd, 0);
}

Zeta_DebugDeque_val_t* Zeta_DebugDeque_PushR(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    return Zeta_DebugDeque_Insert(dd, dd->deque.size());
}

void Zeta_DebugDeque_Erase(void* dd_, size_t idx) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    ZETA_DebugAssert(idx < dd->deque.size());

    dd->deque.erase(dd->deque.begin() + idx);
}

void Zeta_DebugDeque_PopL(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    Zeta_DebugDeque_Erase(dd, 0);
}

void Zeta_DebugDeque_PopR(void* dd_) {
    Zeta_DebugDeque* dd = (Zeta_DebugDeque*)dd_;
    ZETA_DebugAssert(dd != NULL);

    Zeta_DebugDeque_Erase(dd, dd->deque.size() - 1);
}
