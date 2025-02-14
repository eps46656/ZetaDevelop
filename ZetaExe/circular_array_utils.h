#pragma once

#include <circular_array.h>

#include <cstdlib>

#include "seq_cntr_utils.h"

template <typename Elem>
Zeta_SeqCntr CircularArray_Create(size_t stride, size_t capacity) {
    ZETA_DebugAssert(sizeof(Elem) <= stride);
    ZETA_DebugAssert(stride % alignof(Elem) == 0);

    Zeta_CircularArray* ca{ static_cast<Zeta_CircularArray*>(
        std::malloc(sizeof(Zeta_CircularArray))) };

    ca->data = std::malloc(stride * capacity);
    ca->width = sizeof(Elem);
    ca->stride = stride;
    ca->offset = 0;
    ca->size = 0;
    ca->capacity = capacity;

    SeqCntrUtils_AddSanitizeFunc(&zeta_circular_array_seq_cntr_vtable,
                                 CircularArray_Sanitize);

    SeqCntrUtils_AddDestroyFunc(&zeta_circular_array_seq_cntr_vtable,
                                CircularArray_Destroy);

    return { &zeta_circular_array_seq_cntr_vtable, ca };
}

void CircularArray_Destroy(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_circular_array_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }

    Zeta_CircularArray* ca{ static_cast<Zeta_CircularArray*>(
        seq_cntr.context) };

    Zeta_CircularArray_Deinit(ca);

    delete ca;
}

void CircularArray_Sanitize(Zeta_SeqCntr seq_cntr) {
    ZETA_DebugAssert(seq_cntr.vtable == &zeta_circular_array_seq_cntr_vtable);
    if (seq_cntr.context == NULL) { return; }
}
