#pragma once

#include "debugger.h"

// -----------------------------------------------------------------------------

#define ZETA_ColorPtr_GetPtr(color_ptr, align) \
    __builtin_align_down((void*)(*(color_ptr)), (align))

#define ZETA_ColorPtr_GetColor_(tmp, color_ptr, align) \
    ({                                                 \
        unsigned char* tmp = *(color_ptr);             \
        tmp - __builtin_align_down(tmp, (align));      \
    })

#define ZETA_ColorPtr_GetColor(color_ptr, align) \
    ZETA_ColorPtr_GetColor_(ZETA_TmpName, (color_ptr), (align))

#define ZETA_ColorPtr_Set_(tmp_color_ptr, tmp_color, color_ptr, align, ptr, \
                           color)                                           \
    {                                                                       \
        ZETA_AutoVar(tmp_color_ptr, (color_ptr));                           \
        ZETA_AutoVar(tmp_color, (color));                                   \
        ZETA_DebugAssert(0 <= tmp_color &&                                  \
                         (unsigned long long)tmp_color < (align));          \
        *tmp_color_ptr = (void*)((unsigned char*)(void*)(ptr) + tmp_color); \
    }                                                                       \
    ZETA_StaticAssert(TRUE)

#define ZETA_ColorPtr_Set(color_ptr, align, ptr, color)                  \
    ZETA_ColorPtr_Set_(ZETA_TmpName, ZETA_TmpName, (color_ptr), (align), \
                       (ptr), (color))

#define ZETA_ColorPtr_SetPtr_(tmp_color_ptr, tmp_align, tmp_ptr, color_ptr,    \
                              align, ptr)                                      \
    {                                                                          \
        ZETA_AutoVar(tmp_color_ptr, (color_ptr));                              \
        ZETA_AutoVar(tmp_align, (align));                                      \
        ZETA_AutoVar(tmp_ptr, (ptr));                                          \
        ZETA_ColorPtr_Set(tmp_color_ptr, tmp_align, tmp_ptr,                   \
                          (ZETA_ColorPtr_GetColor(tmp_color_ptr, tmp_align))); \
    }                                                                          \
    ZETA_StaticAssert(TRUE)

#define ZETA_ColorPtr_SetPtr(color_ptr, align, ptr)                 \
    ZETA_ColorPtr_SetPtr_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, \
                          (color_ptr), (align), (ptr))

#define ZETA_ColorPtr_SetColor_(tmp_color_ptr, tmp_align, tmp_color,        \
                                color_ptr, align, color)                    \
    {                                                                       \
        ZETA_AutoVar(tmp_color_ptr, (color_ptr));                           \
        ZETA_AutoVar(tmp_align, (align));                                   \
        ZETA_AutoVar(tmp_color, (color));                                   \
        ZETA_ColorPtr_Set(tmp_color_ptr, tmp_align,                         \
                          (ZETA_ColorPtr_GetPtr(tmp_color_ptr, tmp_align)), \
                          tmp_color);                                       \
    }                                                                       \
    ZETA_StaticAssert(TRUE)

#define ZETA_ColorPtr_SetColor(color_ptr, align, color)               \
    ZETA_ColorPtr_SetColor_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName, \
                            (color_ptr), (align), (color))

// -----------------------------------------------------------------------------

#define ZETA_RelPtr_GetPtr(rel_ptr, base) \
    ((void*)((unsigned char*)(void*)(base) + *(rel_ptr)))

#define ZETA_RelPtr_SetPtr_(tmp_rel_ptr, tmp_val, rel_ptr, base, ptr) \
    {                                                                 \
        ZETA_AutoVar(tmp_rel_ptr, (rel_ptr));                         \
        ZETA_AutoVar(tmp_val, ((unsigned char*)(void*)(ptr) -         \
                               (unsigned char*)(void*)(base)));       \
        *tmp_rel_ptr = tmp_val;                                       \
        ZETA_DebugAssert(*tmp_rel_ptr == tmp_val);                    \
    }                                                                 \
    ZETA_StaticAssert(TRUE)

#define ZETA_RelPtr_SetPtr(rel_ptr, base, ptr) \
    ZETA_RelPtr_SetPtr_(ZETA_TmpName, ZETA_TmpName, (rel_ptr), (base), (ptr))

// -----------------------------------------------------------------------------

#define ZETA_RelColorPtr_GetPtr(rel_color_ptr, align, base) \
    __builtin_align_down(                                   \
        (void*)((unsigned char*)(void*)(base) + *(rel_color_ptr)), (align))

#define ZETA_RelColorPtr_GetColor_(tmp, rel_color_ptr, align, base)            \
    ({                                                                         \
        unsigned char* tmp = (unsigned char*)(void*)(base) + *(rel_color_ptr); \
        tmp - __builtin_align_down(tmp, (align));                              \
    })

#define ZETA_RelColorPtr_GetColor(rel_color_ptr, align, base) \
    ZETA_RelColorPtr_GetColor_(ZETA_TmpName, (rel_color_ptr), (align), (base))

#define ZETA_RelColorPtr_Set_(tmp_rel_color_ptr, tmp_color, tmp_val,      \
                              rel_color_ptr, align, base, ptr, color)     \
    {                                                                     \
        ZETA_AutoVar(tmp_rel_color_ptr, (rel_color_ptr));                 \
        ZETA_AutoVar(tmp_color, (color));                                 \
        ZETA_DebugAssert(0 <= tmp_color &&                                \
                         (unsigned long long)tmp_color < (align));        \
        ZETA_AutoVar(tmp_val, ((unsigned char*)(void*)(ptr) + tmp_color - \
                               (unsigned char*)(void*)(base)));           \
        *tmp_rel_color_ptr = tmp_val;                                     \
        ZETA_DebugAssert(*tmp_rel_color_ptr == tmp_val);                  \
    }                                                                     \
    ZETA_StaticAssert(TRUE)

#define ZETA_RelColorPtr_Set(rel_color_ptr, align, base, ptr, color) \
    ZETA_RelColorPtr_Set_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName,  \
                          (rel_color_ptr), (align), (base), (ptr), (color))

#define ZETA_RelColorPtr_SetPtr_(tmp_rel_color_ptr, tmp_align, tmp_base,      \
                                 tmp_ptr, rel_color_ptr, align, base, ptr)    \
    {                                                                         \
        ZETA_AutoVar(tmp_rel_color_ptr, (rel_color_ptr));                     \
        ZETA_AutoVar(tmp_align, (align));                                     \
        ZETA_AutoVar(tmp_base, (base));                                       \
        ZETA_AutoVar(tmp_ptr, (ptr));                                         \
        ZETA_RelColorPtr_Set(tmp_rel_color_ptr, tmp_align, tmp_base, tmp_ptr, \
                             (ZETA_RelColorPtr_GetColor(                      \
                                 tmp_rel_color_ptr, tmp_align, tmp_base)));   \
    }                                                                         \
    ZETA_StaticAssert(TRUE)

#define ZETA_RelColorPtr_SetPtr(rel_color_ptr, align, base, ptr)             \
    ZETA_RelColorPtr_SetPtr_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName,       \
                             ZETA_TmpName, (rel_color_ptr), (align), (base), \
                             (ptr))

#define ZETA_RelColorPtr_SetColor_(tmp_rel_color_ptr, tmp_align, tmp_base,     \
                                   tmp_color, rel_color_ptr, align, base,      \
                                   color)                                      \
    {                                                                          \
        ZETA_AutoVar(tmp_rel_color_ptr, (rel_color_ptr));                      \
        ZETA_AutoVar(tmp_align, (align));                                      \
        ZETA_AutoVar(tmp_base, (base));                                        \
        ZETA_AutoVar(tmp_color, (color));                                      \
        ZETA_RelColorPtr_Set(                                                  \
            tmp_rel_color_ptr, tmp_align, tmp_base,                            \
            (ZETA_RelColorPtr_GetPtr(tmp_rel_color_ptr, tmp_align, tmp_base)), \
            tmp_color);                                                        \
    }                                                                          \
    ZETA_StaticAssert(TRUE)

#define ZETA_RelColorPtr_SetColor(rel_color_ptr, align, base, color)           \
    ZETA_RelColorPtr_SetColor_(ZETA_TmpName, ZETA_TmpName, ZETA_TmpName,       \
                               ZETA_TmpName, (rel_color_ptr), (align), (base), \
                               (color))
