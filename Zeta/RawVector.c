#include "RawVector.h"

void Zeta_RawVector_Init(void *rv_) { Zeta_RawVector_Entrust(rv_, 0, 0, 0); }

void Zeta_RawVector_Check(void *rv_) {
    Zeta_RawVector *rv = rv_;
    ZETA_DEBUG_ASSERT(rv != NULL);

    if (rv->data == NULL) {
        ZETA_DEBUG_ASSERT(rv->size == 0);
    } else {
        ZETA_DEBUG_ASSERT(0 <= rv->size);
    }
}

void *Zeta_RawVector_GetData(void *rv_) {
    Zeta_RawVector *rv = rv_;
    ZETA_DEBUG_ASSERT(rv != NULL);
    return rv->data;
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

void Zeta_RawVector_Entrust(void *rv_, void *data, diff_t stride, diff_t size) {
    Zeta_RawVector *rv = rv_;
    ZETA_DEBUG_ASSERT(rv != NULL);

    ZETA_DEBUG_ASSERT(0 <= size);

    if (data == NULL) { ZETA_DEBUG_ASSERT(size == 0); }

    rv->data = data;
    rv->stride = stride;
    rv->size = size;
}

void *Zeta_RawVector_Access(void *rv_, diff_t idx) {
    Zeta_RawVector *rv = rv_;
    ZETA_DEBUG_ASSERT(rv != NULL);

    ZETA_DEBUG_ASSERT(0 <= idx);
    ZETA_DEBUG_ASSERT(idx < rv->size);

    return (void *)(intptr_t)((intptr_t)(rv->data) + rv->stride * idx);
}

void Zeta_RawVector_ToVector(void *rv_, Zeta_Vector *dst) {
    Zeta_RawVector *rv = rv_;
    ZETA_DEBUG_ASSERT(rv != NULL);

    ZETA_DEBUG_ASSERT(dst != NULL);

    dst->context = rv;
    dst->GetSize = Zeta_RawVector_GetSize;
    dst->Access = Zeta_RawVector_Access;
}
