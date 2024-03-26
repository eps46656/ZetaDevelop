#pragma once

#include "define.h"

typedef struct Zeta_CursorOperator Zeta_CursorOperator;

struct Zeta_CursorOperator {
    void* context;

    size_t cursor_width;

    bool_t (*IsEqual)(void* context, void const* cursor_a,
                      void const* cursor_b);

    int (*Compare)(void* context, void const* cursor_a, void const* cursor_b);

    size_t (*Differ)(void* context, void const* cursor_a, void const* cursor_b);

    void* (*Refer)(void* context, void const* cursor);

    void (*StepL)(void* context, void* cursor);

    void (*StepR)(void* context, void* cursor);

    void (*AdvanceL)(void* context, void* cursor, size_t step);

    void (*AdvanceR)(void* context, void* cursor, size_t step);
};
