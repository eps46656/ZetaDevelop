#pragma once

#include "VectorOpr.h"
#include "define.h"

ZETA_DECL_STRUCT(Zeta_RawVector) {
    void *ptr;
    diff_t stride;
    diff_t size;
};

void Zeta_RawVector_Init(void *rv);

void Zeta_RawVector_Check(void *rv);

void *Zeta_RawVector_GetPtr(void *rv);

diff_t Zeta_RawVector_GetStride(void *rv);

diff_t Zeta_RawVector_GetSize(void *rv);

void Zeta_RawVector_Entrust(void *rv, void *ptr, diff_t stride, diff_t size);

void *Zeta_RawVector_GetRef(void *rv, diff_t idx);

void Zeta_RawVector_GetVectorOpr(struct Zeta_VectorOpr *vec_opr);
