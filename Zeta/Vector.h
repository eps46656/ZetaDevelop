#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_Vector) {
    void *context;

    size_t (*GetWidth)(void *context);

    size_t (*GetSize)(void *context);

    void *(*Access)(void *context, size_t idx);
};
