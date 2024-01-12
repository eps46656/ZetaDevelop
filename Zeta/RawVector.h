#pragma once

#include "Vector.h"

ZETA_DECL_STRUCT(Zeta_RawVector) {
    void *data;    /* the base address of raw vector */
    diff_t stride; /* the number of bytes from the begin of an element to the
                    begin of next element */
    diff_t size;   /* the number of elements this raw vector contains */
};

void Zeta_RawVector_Init(void *rv);

void Zeta_RawVector_Check(void *rv);

void *Zeta_RawVector_GetData(void *rv);

diff_t Zeta_RawVector_GetStride(void *rv);

diff_t Zeta_RawVector_GetSize(void *rv);

void Zeta_RawVector_Entrust(void *rv, void *data, diff_t stride, diff_t size);

void *Zeta_RawVector_Access(void *rv, diff_t idx);

void Zeta_RawVector_ToVector(void *rv, Zeta_Vector *vec);
