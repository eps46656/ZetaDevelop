#pragma once

#include "define.h"

typedef struct Zeta_Vector Zeta_Vector;

struct Zeta_Vector {
    void *context;

    size_t (*GetWidth)(void *context);

    size_t (*GetSize)(void *context);

    void *(*Access)(void *context, size_t idx);
};
