#include "RawVector.h"

void Zeta_RawVector_Init(void* rv_) { Zeta_RawVector_Entrust(rv_, 0, 0, 0); }

void Zeta_RawVector_Check(void* rv_) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    if (rv->data == NULL) {
        ZETA_DebugAssert(rv->size == 0);
    } else {
        ZETA_DebugAssert(0 <= rv->size);
    }
}

void* Zeta_RawVector_GetData(void* rv_) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);
    return rv->data;
}

diff_t Zeta_RawVector_GetStride(void* rv_) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);
    return rv->stride;
}

size_t Zeta_RawVector_GetWidth(void* rv_) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);
    return rv->width;
}

size_t Zeta_RawVector_GetSize(void* rv_) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);
    return rv->size;
}

void Zeta_RawVector_Entrust(void* rv_, void* data, diff_t stride, size_t size) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    ZETA_DebugAssert(0 <= size);

    if (data == NULL) { ZETA_DebugAssert(size == 0); }

    rv->data = data;
    rv->stride = stride;
    rv->size = size;
}

void* Zeta_RawVector_Access(void* rv_, size_t idx) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    ZETA_DebugAssert(0 <= idx);
    ZETA_DebugAssert(idx < rv->size);

    return ZETA_OffsetPtr(rv->data + rv->stride * idx);
}

void* Zeta_RawVector_Insert(void* rv_, size_t idx) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    return Zeta_RawVector_Access(rc, idx);
}

void Zeta_RawVector_Erase(void* rv_, size_t idx) {}

void Zeta_RawVector_ToVector(void* rv_, Zeta_Vector* dst) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = rv;
    dst->GetWidth = Zeta_RawVector_GetWidth;
    dst->GetSize = Zeta_RawVector_GetSize;
    dst->Access = Zeta_RawVector_Access;
    dst->Insert = Zeta_RawVector_Insert;
    dst->Erase = Zeta_RawVector_Erase;
}
