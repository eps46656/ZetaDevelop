#pragma once

#include "CursorOpr.h"
#include "Vector.h"
#include "utils.h"

void Zeta_KthElement(Zeta_Vector* vec, diff_t width, diff_t beg, diff_t mid,
                     diff_t end, void* cmper_context,
                     int (*cmper)(void* cmper_context, const void* x,
                                  const void* y));

void Zeta_Sort(Zeta_Vector* vec, diff_t width, diff_t beg, diff_t end,
               void* cmper_context,
               int (*cmper)(void* cmper_context, const void* x, const void* y));
