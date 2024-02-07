#pragma once

#include "Vector.h"
#include "utils.h"

ZETA_extern_c_beg;

void Zeta_KthElement(Zeta_Vector* vec, size_t beg, size_t mid, size_t end,
                     void* cmper_context,
                     int (*cmper)(void* cmper_context, const void* x,
                                  const void* y));

void Zeta_Sort(Zeta_Vector* vec, size_t beg, size_t end, void* cmper_context,
               int (*cmper)(void* cmper_context, const void* x, const void* y));

ZETA_extern_c_end;
