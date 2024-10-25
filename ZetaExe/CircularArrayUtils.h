#pragma once

#include <CircularArray.h>

#include <cstdlib>

#include "SeqContainerUtils.h"

template <typename Elem>
void CircularArray_Init(Zeta_SeqContainer* seq_cntr, size_t capacity) {
    Zeta_CircularArray* ca{ static_cast<Zeta_CircularArray*>(
        std::malloc(sizeof(Zeta_CircularArray))) };

    ca->data = std::malloc(sizeof(Elem) * capacity);
    ca->width = sizeof(Elem);
    ca->stride = sizeof(Elem);
    ca->offset = 0;
    ca->size = 0;
    ca->capacity = capacity;

    Zeta_CircularArray_DeploySeqContainer(ca, seq_cntr);

    SeqContainerUtils_AddSanitizeFunc(Zeta_CircularArray_GetWidth,
                                      CircularArray_Sanitize);
}

void CircularArray_Deinit(Zeta_SeqContainer* seq_cntr) {
    Zeta_CircularArray* ca{ static_cast<Zeta_CircularArray*>(
        seq_cntr->context) };

    std::free(ca->data);
}

template <typename Elem>
Zeta_SeqContainer* CircularArray_Create(size_t capacity) {
    Zeta_SeqContainer* seq_cntr{ new Zeta_SeqContainer{} };

    CircularArray_Init<Elem>(seq_cntr, capacity);

    return seq_cntr;
}

void CircularArray_Destroy(Zeta_SeqContainer* seq_cntr) {
    ZETA_DebugAssert(seq_cntr->GetWidth == Zeta_CircularArray_GetWidth);

    CircularArray_Deinit(seq_cntr);

    delete seq_cntr;
}

void CircularArray_Sanitize(Zeta_SeqContainer const* seq_cntr) {
    ZETA_DebugAssert(seq_cntr != NULL);
    ZETA_DebugAssert(seq_cntr->GetWidth == Zeta_CircularArray_GetWidth);
}
