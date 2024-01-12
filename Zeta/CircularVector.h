#pragma once

#include "Vector.h"

ZETA_DECL_STRUCT(Zeta_CircularVector) {
    Zeta_Vector *vec;
    diff_t offset;
    diff_t size;
};

void Zeta_CircularVector_Init(void *cv);

void Zeta_CircularVector_Check(void *cv);

void *Zeta_CircularVector_GetVector(void *cv);

diff_t Zeta_CircularVector_GetOffset(void *cv);

diff_t Zeta_CircularVector_GetSize(void *cv);

diff_t Zeta_CircularVector_GetCapacity(void *cv);

void Zeta_CircularVector_Entrust(void *cv, void *vec, diff_t size,
                                 diff_t offset);

void *Zeta_CircularVector_Access(void *cv, diff_t idx);

void *Zeta_CircularVector_PushL(void *cv);

void *Zeta_CircularVector_PushR(void *cv);

void Zeta_CircularVector_ToVector(void *cv, Zeta_Vector *dst);
