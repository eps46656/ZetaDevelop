#pragma once

#include "define.h"

#define ZETA_RelPtr_GetPtr(rel_ptr, base) \
    (void*)((unsigned char*)(void*)(base) + *(rel_ptr))

#define ZETA_RelPtr_SetPtr_(tmp_rel_ptr, tmp_val, rel_ptr, base, ptr)     \
    {                                                                     \
        ZETA_AutoVar(tmp_rel_ptr, (rel_ptr));                             \
        long long tmp_val =                                               \
            (unsigned char*)(void*)(ptr) - (unsigned char*)(void*)(base); \
        *tmp_rel_ptr = tmp_val;                                           \
        ZETA_DebugAssert(*tmp_rel_ptr == tmp_val);                        \
    }                                                                     \
    ZETA_StaticAssert(TRUE)

#define ZETA_RelPtr_SetPtr(rel_ptr, base, ptr) \
    ZETA_RelPtr_SetPtr_(ZETA_TmpName, ZETA_TmpName, (rel_ptr), (base), (ptr))

// -----------------------------------------------------------------------------

#define ZETA_RelColorPtr_GetPtr(rel_ptr, align, base) \
    __builtin_align_down(ZETA_RelPtr_GetPtr((rel_ptr), (base)), (align))

#define ZETA_RelColorPtr_GetColor_(tmp, rel_ptr, align, base)       \
    ({                                                              \
        unsigned char* tmp = ZETA_RelPtr_GetPtr((rel_ptr), (base)); \
        tmp - __builtin_align_down(tmp, (align));                   \
    })

#define ZETA_RelColorPtr_GetColor(rel_ptr, align, base) \
    ZETA_RelColorPtr_GetColor_(ZETA_TmpName, (rel_ptr), (align), (base))

#define ZETA_RelColorPtr_Set_(tmp_color, rel_ptr, align, base, ptr, color) \
    {                                                                      \
        ZETA_AutoVar(tmp_color, color);                                    \
        ZETA_DebugAssert(0 <= tmp_color && (size_t)tmp_color < (align));   \
        ZETA_RelPtr_SetPtr((rel_ptr), (base),                              \
                           (unsigned char*)(void*)(ptr) + tmp_color);      \
    }                                                                      \
    ZETA_StaticAssert(TRUE)

#define ZETA_RelColorPtr_Set(rel_ptr, align, base, ptr, color)             \
    ZETA_RelColorPtr_Set_(ZETA_TmpName, (rel_ptr), (align), (base), (ptr), \
                          (color))

#define ZETA_RelColorPtr_SetPtr(rel_ptr, align, base, ptr) \
    ZETA_RelColorPtr_Set(                                  \
        (rel_ptr), (align), (base), (ptr),                 \
        ZETA_RelColorPtr_GetColor((rel_ptr), (align), (base)))

#define ZETA_RelColorPtr_SetColor(rel_ptr, align, base, color)                \
    ZETA_RelColorPtr_Set((rel_ptr), (align), (base),                          \
                         ZETA_RelColorPtr_GetPtr((rel_ptr), (align), (base)), \
                         (color))
