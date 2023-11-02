#pragma once

#include "CircularVector.h"

void Zeta_CircularVector_Init(void *cv_) {
    Zeta_CircularVector *cv = cv_;
    Zeta_CircularVector_Entrust(cv, NULL, 0, 0);
}

void Zeta_CircularVector_Check(void *cv_) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    Zeta_VectorOpr *vec_opr = &cv->vec_opr;

    if (cv->vector == NULL) {
        ZETA_DEBUG_ASSERT(cv->size == 0);
        ZETA_DEBUG_ASSERT(cv->offset == 0);
        return;
    }

    ZETA_DEBUG_ASSERT(vec_opr->GetSize != NULL);

    diff_t capacity = vec_opr->GetSize(cv->vector);
    diff_t size = cv->size;
    diff_t offset = cv->offset;

    ZETA_DEBUG_ASSERT(0 <= offset);
    ZETA_DEBUG_ASSERT(offset < capacity);
    ZETA_DEBUG_ASSERT(size <= capacity);
}

void *Zeta_CircularVector_GetVector(void *cv_) {
    struct Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);
    return cv->vector;
}

diff_t Zeta_CircularVector_GetCapacity(void *cv_) {
    struct Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    if (cv->vector == NULL) { return 0; }

    Zeta_VectorOpr *vec_opr = &cv->vec_opr;
    ZETA_DEBUG_ASSERT(vec_opr->GetSize != NULL);

    return vec_opr->GetSize(cv->vector);
}

diff_t Zeta_CircularVector_GetSize(void *cv_) {
    struct Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);
    return cv->size;
}

diff_t Zeta_CircularVector_GetOffset(void *cv_) {
    struct Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);
    return cv->offset;
}

void Zeta_CircularVector_Entrust(void *cv_, void *vector, diff_t size,
                                 diff_t offset) {
    struct Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    if (vector == NULL) {
        ZETA_DEBUG_ASSERT(size == 0);
        ZETA_DEBUG_ASSERT(offset == 0);

        cv->vector = NULL;
        cv->size = 0;
        cv->offset = 0;
    } else {
        Zeta_VectorOpr *vec_opr = &cv->vec_opr;
        ZETA_DEBUG_ASSERT(vec_opr->GetSize != NULL);

        diff_t capacity = vec_opr->GetSize(vector);

        ZETA_DEBUG_ASSERT(0 <= offset);
        ZETA_DEBUG_ASSERT(offset < capacity);
        ZETA_DEBUG_ASSERT(size <= capacity);

        cv->vector = vector;
        cv->size = size;
        cv->offset = offset;
    }
}

void *Zeta_CircularVector_GetRef(void *cv_, diff_t idx) {
    struct Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    Zeta_VectorOpr *vec_opr = &cv->vec_opr;
    ZETA_DEBUG_ASSERT(vec_opr->GetSize != NULL);

    diff_t capacity = vec_opr->GetSize(cv->vector);
    diff_t size = cv->size;
    diff_t offset = cv->offset;

    ZETA_DEBUG_ASSERT(0 <= offset);
    ZETA_DEBUG_ASSERT(offset < capacity);
    ZETA_DEBUG_ASSERT(size <= capacity);

    ZETA_DEBUG_ASSERT(0 <= idx && idx < size);
    ZETA_DEBUG_ASSERT(vec_opr->GetRef != NULL);

    return vec_opr->GetRef(cv->vector, (offset + idx) % capacity);
}

void *Zeta_CircularVector_PushL(void *cv_) {
    struct Zeta_CircularVector *cv = cv_;

    ZETA_DEBUG_ASSERT(cv != NULL);

    Zeta_VectorOpr *vec_opr = &cv->vec_opr;
    ZETA_DEBUG_ASSERT(vec_opr->GetSize != NULL);

    diff_t capacity = vec_opr->GetSize(cv->vector);
    diff_t size = cv->size;
    diff_t offset = cv->offset;

    ZETA_DEBUG_ASSERT(0 <= offset);
    ZETA_DEBUG_ASSERT(offset < capacity);
    ZETA_DEBUG_ASSERT(size < capacity);

    cv->offset = (offset + capacity - 1) % capacity;
    ++cv->size;

    return Zeta_CircularVector_GetRef(cv, 0);
}

void *Zeta_CircularVector_PushR(void *cv_) {
    struct Zeta_CircularVector *cv = cv_;

    ZETA_DEBUG_ASSERT(cv != NULL);

    Zeta_VectorOpr *vec_opr = &cv->vec_opr;
    ZETA_DEBUG_ASSERT(vec_opr->GetSize != NULL);

    diff_t capacity = vec_opr->GetSize(cv->vector);
    diff_t size = cv->size;
    diff_t offset = cv->offset;

    ZETA_DEBUG_ASSERT(0 <= offset);
    ZETA_DEBUG_ASSERT(offset < capacity);
    ZETA_DEBUG_ASSERT(size < capacity);

    ++cv->size;

    return Zeta_CircularVector_GetRef(cv, cv->size - 1);
}
