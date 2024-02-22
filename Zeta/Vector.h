#pragma once

#include "define.h"

typedef struct Zeta_Vector Zeta_Vector;

struct Zeta_Vector {
    void* context;

    size_t (*GetWidth)(void* context);

    size_t (*GetSize)(void* context);

    void* (*Access)(void* context, size_t idx);

    void* (*PeekL)(void* context);

    void* (*PeekR)(void* context);

    void* (*Insert)(void* context, size_t idx);

    void* (*PushL)(void* context);

    void* (*PushR)(void* context);

    void (*Erase)(void* context, size_t idx);

    void (*PopL)(void* context);

    void (*PopR)(void* context);
};
