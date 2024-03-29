#pragma once

#include "Vector.h"

ZETA_extern_c_beg;

typedef struct Zeta_RawVector Zeta_RawVector;

struct Zeta_RawVector {
    /** the base address of raw vector */
    void* data;

    /** The width(byte) of single element occupying. */
    size_t width;

    /** The offset(byte) from the beginning of an element to the beginning of
     * next element */
    diff_t stride;

    /** The number of elements this raw vector contains */
    size_t size;
};

void Zeta_RawVector_Init(void* rv);

void Zeta_RawVector_Check(void* rv);

void* Zeta_RawVector_GetData(void* rv);

diff_t Zeta_RawVector_GetStride(void* rv);

size_t Zeta_RawVector_GetWidth(void* rv);

size_t Zeta_RawVector_GetSize(void* rv);

void* Zeta_RawVector_PeekL(void* rv);

void* Zeta_RawVector_PeekR(void* rv);

void* Zeta_RawVector_Access(void* rv, size_t idx);

void Zeta_RawVector_ToVector(void* rv, Zeta_Vector* vec);

ZETA_extern_c_end;
