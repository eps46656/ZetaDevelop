#pragma once

#include "Vector.h"

ZETA_DECL_STRUCT(Zeta_CircularVector) {
    Zeta_Vector *vec;
    size_t offset;
    size_t size;
};

void Zeta_CircularVector_Init(void *cv);

void Zeta_CircularVector_Check(void *cv);

void *Zeta_CircularVector_GetVector(void *cv);

size_t Zeta_CircularVector_GetWidth(void *cv);

size_t Zeta_CircularVector_GetOffset(void *cv);

size_t Zeta_CircularVector_GetSize(void *cv);

size_t Zeta_CircularVector_GetCapacity(void *cv);

void Zeta_CircularVector_Entrust(void *cv, void *vec, size_t size,
                                 size_t offset);

void *Zeta_CircularVector_Access(void *cv, size_t idx);

void *Zeta_CircularVector_Insert(void *cv_, size_t idx);

void *Zeta_CircularVector_PushL(void *cv);

void *Zeta_CircularVector_PushR(void *cv);

void Zeta_CircularVector_Erase(void *cv, size_t idx);

void Zeta_CircularVector_PopL(void *cv);

void Zeta_CircularVector_PopR(void *cv);

void Zeta_CircularVector_ToVector(void *cv, Zeta_Vector *dst);
