#pragma once

#include <cstdlib>

#include "../Zeta/CircularArray.h"

template <typename Val>
void InitCircularArray(Zeta_SeqContainer* seq_cntr, size_t capacity) {
    Zeta_CircularArray* ca{ static_cast<Zeta_CircularArray*>(
        std::malloc(sizeof(Zeta_CircularArray))) };

    ca->data = std::malloc(sizeof(Val) * capacity);
    ca->width = sizeof(Val);
    ca->stride = sizeof(Val);
    ca->offset = 0;
    ca->size = 0;
    ca->capacity = capacity;

    Zeta_CircularArray_DeploySeqContainer(ca, seq_cntr);
}

template <typename Val>
Zeta_SeqContainer* CreateCircularArray(size_t capacity) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };

    InitCircularArray<Val>(seq_cntr, capacity);

    return seq_cntr;
}
