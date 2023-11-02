#include "RawVector.h"

void Zeta_RawVector_Init(void *rv_) { Zeta_RawVector_Entrust(rv_, 0, 0, 0); }

void Zeta_RawVector_Check(void *rv_) {
    Zeta_RawVector *rv = rv_;
    ZETA_DEBUG_ASSERT(rv != NULL);

    if (rv->ptr == NULL) {
        ZETA_DEBUG_ASSERT(rv->size == 0);
    } else {
        ZETA_DEBUG_ASSERT(0 <= rv->size);
    }
}

void *Zeta_RawVector_GetPtr(void *rv_) {
    Zeta_RawVector *rv = rv_;
    ZETA_DEBUG_ASSERT(rv != NULL);
    return rv->ptr;
}

diff_t Zeta_RawVector_GetStride(void *rv_) {
    Zeta_RawVector *rv = rv_;
    ZETA_DEBUG_ASSERT(rv != NULL);
    return rv->stride;
}

diff_t Zeta_RawVector_GetSize(void *rv_) {
    Zeta_RawVector *rv = rv_;
    ZETA_DEBUG_ASSERT(rv != NULL);
    return rv->size;
}

void Zeta_RawVector_Entrust(void *rv_, void *ptr, diff_t stride, diff_t size) {
    Zeta_RawVector *rv = rv_;

    ZETA_DEBUG_ASSERT(rv != NULL);
    ZETA_DEBUG_ASSERT(0 <= size);

    if (ptr == NULL) { ZETA_DEBUG_ASSERT(size == 0); }

    rv->ptr = ptr;
    rv->stride = stride;
    rv->size = size;
}

void *Zeta_RawVector_GetRef(void *rv_, diff_t idx) {
    Zeta_RawVector *rv = rv_;

    ZETA_DEBUG_ASSERT(rv != NULL);
    ZETA_DEBUG_ASSERT(0 <= idx && idx < rv->size);

    return (void *)((diff_t)(rv->ptr) + rv->stride * idx);
}

void Zeta_RawVector_GetVectorOpr(struct Zeta_VectorOpr *vec_opr) {
    vec_opr->GetSize = Zeta_RawVector_GetSize;
    vec_opr->GetRef = Zeta_RawVector_GetRef;
}
