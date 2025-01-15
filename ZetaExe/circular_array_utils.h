#pragma once

#include <circular_array.h>

#include <cstdlib>

#include "seq_cntr_utils.h"

template <typename Elem>
void CircularArray_Init(Zeta_SeqCntr* seq_cntr, size_t capacity) {
    Zeta_CircularArray* ca{ static_cast<Zeta_CircularArray*>(
        std::malloc(sizeof(Zeta_CircularArray))) };

    ca->data = std::malloc(sizeof(Elem) * capacity);
    ca->width = sizeof(Elem);
    ca->offset = 0;
    ca->size = 0;
    ca->capacity = capacity;

    Zeta_CircularArray_DeploySeqCntr(ca, seq_cntr);

    SeqCntrUtils_AddSanitizeFunc(Zeta_CircularArray_GetWidth,
                                 CircularArray_Sanitize);
}

void CircularArray_Deinit(Zeta_SeqCntr* seq_cntr) {
    Zeta_CircularArray* ca{ static_cast<Zeta_CircularArray*>(
        seq_cntr->context) };

    std::free(ca->data);
}

template <typename Elem>
Zeta_SeqCntr* CircularArray_Create(size_t capacity) {
    Zeta_SeqCntr* seq_cntr{ new Zeta_SeqCntr{} };

    CircularArray_Init<Elem>(seq_cntr, capacity);

    return seq_cntr;
}

void CircularArray_Destroy(Zeta_SeqCntr* seq_cntr) {
    ZETA_DebugAssert(seq_cntr != NULL);
    ZETA_DebugAssert(seq_cntr->GetWidth == Zeta_CircularArray_GetWidth);

    CircularArray_Deinit(seq_cntr);

    delete seq_cntr;
}

void CircularArray_Sanitize(Zeta_SeqCntr const* seq_cntr) {
    ZETA_DebugAssert(seq_cntr != NULL);
    ZETA_DebugAssert(seq_cntr->GetWidth == Zeta_CircularArray_GetWidth);
}
