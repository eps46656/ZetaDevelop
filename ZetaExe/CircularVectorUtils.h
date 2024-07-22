#pragma once

#include <cstdlib>

#include "../Zeta/CircularVector.h"

template <typename Val>
void InitCircularVector(Zeta_SeqContainer* seq_cntr, size_t capacity) {
    Zeta_CircularVector* cv{ static_cast<Zeta_CircularVector*>(
        std::malloc(sizeof(Zeta_CircularVector))) };

    cv->data = std::malloc(sizeof(Val) * capacity);
    cv->width = sizeof(Val);
    cv->stride = sizeof(Val);
    cv->offset = 0;
    cv->size = 0;
    cv->capacity = capacity;

    Zeta_CircularVector_DeploySeqContainer(cv, seq_cntr);
}

template <typename Val>
Zeta_SeqContainer* CreateCircularVector(size_t capacity) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };

    InitCircularVector<Val>(seq_cntr, capacity);

    return seq_cntr;
}
