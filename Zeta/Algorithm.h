#pragma once

#include "utils.h"

ZETA_ExternC_Beg;

void Zeta_KthElement(void* data, size_t width, size_t stride, size_t mid,
                     size_t size, void* cmp_context,
                     int (*Compare)(void* cmp_context, void const* x,
                                    void const* y));

void Zeta_Sort(void* data, size_t width, size_t stride, size_t size,
               void* cmp_context,
               int (*Compare)(void* cmp_context, void const* x, void const* y));

ZETA_ExternC_End;
