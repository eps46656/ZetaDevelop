#pragma once

#include "Vector.h"

typedef struct Zeta_CircularVector Zeta_CircularVector;

struct Zeta_CircularVector {
    Zeta_Vector *vec;
    size_t offset;
    size_t size;
};

EXTERN_C void Zeta_CircularVector_Init(void *cv);

EXTERN_C void Zeta_CircularVector_Check(void *cv);

EXTERN_C void *Zeta_CircularVector_GetVector(void *cv);

EXTERN_C size_t Zeta_CircularVector_GetWidth(void *cv);

EXTERN_C size_t Zeta_CircularVector_GetOffset(void *cv);

EXTERN_C size_t Zeta_CircularVector_GetSize(void *cv);

EXTERN_C size_t Zeta_CircularVector_GetCapacity(void *cv);

EXTERN_C void Zeta_CircularVector_Entrust(void *cv, void *vec, size_t size,
                                          size_t offset);

EXTERN_C void *Zeta_CircularVector_Access(void *cv, size_t idx);

EXTERN_C void *Zeta_CircularVector_Insert(void *cv_, size_t idx);

EXTERN_C void *Zeta_CircularVector_PushL(void *cv);

EXTERN_C void *Zeta_CircularVector_PushR(void *cv);

EXTERN_C void Zeta_CircularVector_Erase(void *cv, size_t idx);

EXTERN_C void Zeta_CircularVector_PopL(void *cv);

EXTERN_C void Zeta_CircularVector_PopR(void *cv);

EXTERN_C void Zeta_CircularVector_ToVector(void *cv, Zeta_Vector *dst);
