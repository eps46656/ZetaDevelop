#pragma once

#include "test_head.h"

typedef struct SimpleVector SimpleVector;

struct SimpleVector {
    size_t capacity;
    size_t size;
    val_t* data;
};

void SimpleVector_Init(SimpleVector* sv) {
    sv->capacity = 0;
    sv->size = 0;
    sv->data = NULL;
}

val_t* SimpleVector_Access(SimpleVector* sv, size_t idx) {
    ZETA_DebugAssert(0 <= idx);
    ZETA_DebugAssert(idx < sv->size);
    return sv->data + idx;
}

void SimpleVector_Reserve(SimpleVector* sv, size_t capacity) {
    if (capacity <= sv->capacity) { return; }

    val_t* new_data = malloc(sizeof(val_t) * capacity);
    memcpy(new_data, sv->data, sizeof(val_t) * sv->size);
    free(sv->data);

    sv->capacity = capacity;
    sv->data = new_data;
}

val_t* SimpleVector_InsertWithIdx(SimpleVector* sv, size_t idx) {
    ZETA_DebugAssert(0 <= idx);
    ZETA_DebugAssert(idx <= sv->size);

    if (sv->capacity == sv->size) {
        size_t new_capacity = sv->capacity == 0 ? 8 : sv->capacity * 2;
        val_t* new_data = malloc(sizeof(val_t) * new_capacity);

        memcpy(new_data, sv->data, sizeof(val_t) * idx);
        memcpy(new_data + idx + 1, sv->data + idx,
               sizeof(val_t) * (sv->size - idx));
        free(sv->data);

        sv->capacity = new_capacity;
        sv->data = new_data;
    } else {
        memmove(sv->data + idx + 1, sv->data + idx,
                sizeof(val_t) * (sv->size - idx));
    }

    ++sv->size;
    return sv->data + idx;
}

void SimpleVector_EraseWithIdx(SimpleVector* sv, size_t idx) {
    ZETA_DebugAssert(0 <= idx);
    ZETA_DebugAssert(idx < sv->size);

    memmove(sv->data + idx, sv->data + idx + 1,
            sizeof(val_t) * (sv->size - idx - 1));

    --sv->size;
}

void SimpleVector_PrintAll(SimpleVector* sv) {
    printf("{ ");
    for (size_t i = 0; i < sv->size; ++i) { printf("%d ", sv->data[i]); }
    printf("}\n");
}
