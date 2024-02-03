#pragma once

#include "CursorOpr.h"
#include "Vector.h"
#include "utils.h"

EXTERN_C void Zeta_KthElement(Zeta_Vector* vec, size_t beg, size_t mid,
                              size_t end, void* cmper_context,
                              int (*cmper)(void* cmper_context, const void* x,
                                           const void* y));

EXTERN_C void Zeta_Sort(Zeta_Vector* vec, size_t beg, size_t end,
                        void* cmper_context,
                        int (*cmper)(void* cmper_context, const void* x,
                                     const void* y));
