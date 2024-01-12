#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_CursorOpr) {
    void* context;

    void* (*Access)(void* context, void* cursor);

    void* (*Advance)(void* context, void* cursor, diff_t step);

    diff_t (*GetDistance)(void* context, const void* cursor_a,
                          const void* cursor_b);
};
