#pragma once

#include "define.h"

typedef struct Zeta_CursorOpr Zeta_CursorOpr;

struct Zeta_CursorOpr {
    void* context;

    void* (*Access)(void* context, void* cursor);

    void* (*Advance)(void* context, void* cursor, diff_t step);

    diff_t (*GetDistance)(void* context, const void* cursor_a,
                          const void* cursor_b);
};
