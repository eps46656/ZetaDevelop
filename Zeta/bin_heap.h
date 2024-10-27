#pragma once

#include "define.h"

ZETA_ExternC_Beg;

void Zeta_BinHeap_Construct(void* data, size_t width, size_t stride,
                            size_t size, void* cmp_context,
                            int (*Compare)(void* cmp_context, void const* x,
                                           void const* y));

void Zeta_BinHeap_Push(void* data, size_t width, size_t stride, size_t size,
                       void const* src_elem, void* cmp_context,
                       int (*Compare)(void* cmp_context, void const* x,
                                      void const* y));

void Zeta_BinHeap_Pop(void* data, size_t width, size_t stride, size_t size,
                      void* dst_elem, void* cmp_context,
                      int (*Compare)(void* cmp_context, void const* x,
                                     void const* y));

ZETA_ExternC_End;
