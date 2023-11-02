#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_VectorOpr) {
    diff_t (*GetSize)(void *cntr);
    void *(*GetRef)(void *cntr, diff_t idx);
};
