#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_Vector) {
    void *context;

    diff_t (*GetSize)(void *context);

    void *(*Access)(void *context, diff_t idx);
};
