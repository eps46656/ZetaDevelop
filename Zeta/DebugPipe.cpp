#include "DebugPipe.h"

#include <deque>

void Zeta_DebugPipe_Create(void* dd_) {
    Zeta_DebugPipe* dd{ (Zeta_DebugPipe*)dd_ };
    ZETA_DebugAssert(dd != NULL);

    dd->deque = new std::deque<Zeta_DebugPipe_val_t>;
}

void Zeta_DebugPipe_Destroy(void* dd_) {
    Zeta_DebugPipe* dd{ (Zeta_DebugPipe*)dd_ };
    ZETA_DebugAssert(dd != NULL);

    delete (std::deque<Zeta_DebugPipe_val_t>*)dd->deque;
}

size_t Zeta_DebugPipe_GetSize(void* dd_) {
    Zeta_DebugPipe* dd{ (Zeta_DebugPipe*)dd_ };
    ZETA_DebugAssert(dd != NULL);

    std::deque<Zeta_DebugPipe_val_t>* deque{
        (std::deque<Zeta_DebugPipe_val_t>*)dd->deque
    };

    return deque->size();
}

Zeta_DebugPipe_val_t Zeta_DebugPipe_Peek(void* dd_) {
    Zeta_DebugPipe* dd{ (Zeta_DebugPipe*)dd_ };
    ZETA_DebugAssert(dd != NULL);

    std::deque<Zeta_DebugPipe_val_t>* deque{
        (std::deque<Zeta_DebugPipe_val_t>*)dd->deque
    };

    ZETA_DebugAssert(!deque->empty());

    return deque->front();
}

void Zeta_DebugPipe_Push(void* dd_, Zeta_DebugPipe_val_t val) {
    Zeta_DebugPipe* dd{ (Zeta_DebugPipe*)dd_ };
    ZETA_DebugAssert(dd != NULL);

    std::deque<Zeta_DebugPipe_val_t>* deque{
        (std::deque<Zeta_DebugPipe_val_t>*)dd->deque
    };

    deque->push_back(val);
}

Zeta_DebugPipe_val_t Zeta_DebugPipe_Pop(void* dd_) {
    Zeta_DebugPipe* dd{ (Zeta_DebugPipe*)dd_ };
    ZETA_DebugAssert(dd != NULL);

    std::deque<Zeta_DebugPipe_val_t>* deque{
        (std::deque<Zeta_DebugPipe_val_t>*)dd->deque
    };

    ZETA_DebugAssert(!deque->empty());

    Zeta_DebugPipe_val_t ret{ deque->front() };
    deque->pop_front();

    return ret;
}
