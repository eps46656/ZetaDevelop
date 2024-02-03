#pragma once

#include "Vector.h"

typedef struct Zeta_RawVector Zeta_RawVector;

struct Zeta_RawVector {
    /** the base address of raw vector */
    void *data;

    /** The width(byte) of single element occupying. */
    size_t width;

    /** The offset(byte) from the beginning of an element to the beginning of
     * next element */
    diff_t stride;

    /** The number of elements this raw vector contains */
    size_t size;
};

EXTERN_C void Zeta_RawVector_Init(void *rv);

EXTERN_C void Zeta_RawVector_Check(void *rv);

EXTERN_C void *Zeta_RawVector_GetData(void *rv);

EXTERN_C diff_t Zeta_RawVector_GetStride(void *rv);

EXTERN_C size_t Zeta_RawVector_GetWidth(void *rv);

EXTERN_C size_t Zeta_RawVector_GetSize(void *rv);

EXTERN_C void Zeta_RawVector_Entrust(void *rv, void *data, diff_t stride,
                                     size_t size);

EXTERN_C void *Zeta_RawVector_Access(void *rv, size_t idx);

EXTERN_C void Zeta_RawVector_ToVector(void *rv, Zeta_Vector *vec);
