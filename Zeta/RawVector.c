#include "RawVector.h"

void Zeta_RawVector_Init(void* rv_) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    rv->data = NULL;
    rv->width = 0;
    rv->stride = 0;
    rv->size = 0;
}

void Zeta_RawVector_Check(void* rv_) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    if (rv->data == NULL) { ZETA_DebugAssert(rv->size == 0); }
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

void* Zeta_RawVector_PeekL(void* rv_) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    ZETA_DebugAssert(0 < rv->size);

    return Zeta_RawVector_Access(rv, 0);
}

void* Zeta_RawVector_PeekR(void* rv_) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    ZETA_DebugAssert(0 < rv->size);

    return Zeta_RawVector_Access(rv, rv->size - 1);
}

void* Zeta_RawVector_Access(void* rv_, size_t idx) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    ZETA_DebugAssert(idx < rv->size);

    return ZETA_GetPtrFromAddr(ZETA_GetAddrFromPtr(rv->data) +
                               rv->stride * idx);
}

/*
void* Zeta_RawVector_Insert(void* rv_, size_t idx) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    return Zeta_RawVector_Access(rv, idx);
}

void Zeta_RawVector_Erase(void* rv_, size_t idx) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    ZETA_DebugAssert(idx < rv->size);
}
*/

void Zeta_RawVector_ToVector(void* rv_, Zeta_Vector* dst) {
    Zeta_RawVector* rv = rv_;
    ZETA_DebugAssert(rv != NULL);

    ZETA_DebugAssert(dst != NULL);

    dst->context = rv;
    dst->GetWidth = Zeta_RawVector_GetWidth;
    dst->GetSize = Zeta_RawVector_GetSize;
    dst->PeekL = Zeta_RawVector_PeekL;
    dst->PeekR = Zeta_RawVector_PeekR;
    dst->Access = Zeta_RawVector_Access;
    dst->PushL = NULL;
    dst->PushR = NULL;
    dst->Insert = NULL;
    dst->PopL = NULL;
    dst->PopR = NULL;
    dst->Erase = NULL;
}
