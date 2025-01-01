#pragma once

#include "utils.h"

ZETA_ExternC_Beg;

void* Zeta_NicePartition(void* data, size_t width, size_t stride, size_t size,
                         void* cmp_context, Zeta_Compare Compare);

void Zeta_KthElement(void* data, size_t width, size_t stride, size_t mid,
                     size_t size, void* cmp_context, Zeta_Compare Compare);

void Zeta_Sort(void* data, size_t width, size_t stride, size_t size,
               void* cmp_context, Zeta_Compare Compare);

void Zeta_MergeSort(void* data, size_t width, size_t stride, size_t size,
                    void* cmp_context, Zeta_Compare Compare);

void Zeta_TransMergeSort(void* data, size_t width, size_t stride, size_t size,
                         void* cmp_context, Zeta_Compare Compare);

ZETA_ExternC_End;
