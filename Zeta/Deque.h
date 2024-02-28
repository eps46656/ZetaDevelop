#pragma once

#include "define.h"

typedef struct Zeta_Deque Zeta_Deque;

struct Zeta_Deque {
    void* context;

    size_t (*GetSize)(void* context);

    size_t (*GetCapacity)(void* context);

    void* (*PeekL)(void* context);

    void* (*PeekR)(void* context);

    void (*PushL)(void* context, void const* x);

    void (*PushR)(void* context, void const* x);

    void (*PopL)(void* context);

    void (*PopR)(void* context);
};
