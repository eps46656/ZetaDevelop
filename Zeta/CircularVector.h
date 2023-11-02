#pragma once

#include "VectorOpr.h"

ZETA_DECL_STRUCT(Zeta_CircularVector) {
    Zeta_VectorOpr vec_opr;
    void *vector;
    diff_t size;
    diff_t offset;
};

void Zeta_CircularVector_Init(void *cv);

void Zeta_CircularVector_Check(void *cv);

void *Zeta_CircularVector_GetVector(void *cv);

diff_t Zeta_CircularVector_GetCapacity(void *cv);

diff_t Zeta_CircularVector_GetSize(void *cv);

diff_t Zeta_CircularVector_GetOffset(void *cv);

void Zeta_CircularVector_Entrust(void *cv, void *vector, diff_t size,
                                 diff_t offset);

void *Zeta_CircularVector_GetRef(void *cv, diff_t idx);

void *Zeta_CircularVector_PushL(void *cv);

void *Zeta_CircularVector_PushR(void *cv);
