#pragma once

#include "CircularVector.h"

void Zeta_CircularVector_Init(void *cv_) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    cv->vec = NULL;
    cv->offset = 0;
    cv->size = 0;
}

void Zeta_CircularVector_Check(void *cv_) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    Zeta_Vector *vec = cv->vec;

    if (cv->vec == NULL) {
        ZETA_DEBUG_ASSERT(cv->offset == 0);
        ZETA_DEBUG_ASSERT(cv->size == 0);
        return;
    }

    ZETA_DEBUG_ASSERT(vec->GetSize != NULL);

    diff_t capacity = vec->GetSize(vec->context);
    diff_t offset = cv->offset;
    diff_t size = cv->size;

    ZETA_DEBUG_ASSERT(0 <= offset);
    ZETA_DEBUG_ASSERT(offset < capacity);
    ZETA_DEBUG_ASSERT(size <= capacity);
}

void *Zeta_CircularVector_GetVector(void *cv_) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);
    return cv->vec;
}

diff_t Zeta_CircularVector_GetOffset(void *cv_) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);
    return cv->offset;
}

diff_t Zeta_CircularVector_GetSize(void *cv_) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);
    return cv->size;
}

diff_t Zeta_CircularVector_GetCapacity(void *cv_) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    if (cv->vec == NULL) { return 0; }

    Zeta_Vector *vec = cv->vec;
    ZETA_DEBUG_ASSERT(vec->GetSize != NULL);

    return vec->GetSize(vec->context);
}

void Zeta_CircularVector_Entrust(void *cv_, void *vec, diff_t size,
                                 diff_t offset) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    if (vec == NULL) {
        ZETA_DEBUG_ASSERT(offset == 0);
        ZETA_DEBUG_ASSERT(size == 0);

        cv->vec = NULL;
        cv->offset = 0;
        cv->size = 0;
    } else {
        Zeta_Vector *vec = cv->vec;
        ZETA_DEBUG_ASSERT(vec->GetSize != NULL);

        diff_t capacity = vec->GetSize(vec->context);

        ZETA_DEBUG_ASSERT(0 <= offset);
        ZETA_DEBUG_ASSERT(offset < capacity);
        ZETA_DEBUG_ASSERT(size <= capacity);

        cv->vec = vec;
        cv->offset = offset;
        cv->size = size;
    }
}

void *Zeta_CircularVector_Access(void *cv_, diff_t idx) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    Zeta_Vector *vec = cv->vec;
    ZETA_DEBUG_ASSERT(vec != NULL);
    ZETA_DEBUG_ASSERT(vec->GetSize != NULL);

    diff_t capacity = vec->GetSize(vec->context);
    diff_t offset = cv->offset;
    diff_t size = cv->size;

    ZETA_DEBUG_ASSERT(0 <= offset);
    ZETA_DEBUG_ASSERT(offset < capacity);
    ZETA_DEBUG_ASSERT(size <= capacity);

    ZETA_DEBUG_ASSERT(0 <= idx)
    ZETA_DEBUG_ASSERT(idx < size);
    ZETA_DEBUG_ASSERT(vec->Access != NULL);

    return vec->Access(vec->context, (offset + idx) % capacity);
}

void *Zeta_CircularVector_PushL(void *cv_) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    Zeta_Vector *vec = cv->vec;
    ZETA_DEBUG_ASSERT(vec != NULL);
    ZETA_DEBUG_ASSERT(vec->GetSize != NULL);

    diff_t capacity = vec->GetSize(vec->context);
    diff_t offset = cv->offset;
    diff_t size = cv->size;

    ZETA_DEBUG_ASSERT(0 <= offset);
    ZETA_DEBUG_ASSERT(offset < capacity);
    ZETA_DEBUG_ASSERT(size < capacity);

    cv->offset = (offset + capacity - 1) % capacity;
    ++cv->size;

    return Zeta_CircularVector_Access(cv, 0);
}

void *Zeta_CircularVector_PushR(void *cv_) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    Zeta_Vector *vec = cv->vec;
    ZETA_DEBUG_ASSERT(vec != NULL);
    ZETA_DEBUG_ASSERT(vec->GetSize != NULL);

    diff_t capacity = vec->GetSize(vec->context);
    diff_t offset = cv->offset;
    diff_t size = cv->size;

    ZETA_DEBUG_ASSERT(0 <= offset);
    ZETA_DEBUG_ASSERT(offset < capacity);
    ZETA_DEBUG_ASSERT(size < capacity);

    ++cv->size;

    return Zeta_CircularVector_Access(cv, cv->size - 1);
}

void Zeta_CircularVector_ToVector(void *cv_, Zeta_Vector *dst) {
    Zeta_CircularVector *cv = cv_;
    ZETA_DEBUG_ASSERT(cv != NULL);

    ZETA_DEBUG_ASSERT(dst != NULL);

    dst->context = cv;
    dst->GetSize = Zeta_CircularVector_GetSize;
    dst->Access = Zeta_CircularVector_Access;
}
