#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_BinTreeNodeAccessor) {
    void *context;
    void *(*GetP)(void *context, void *n);
    void *(*GetL)(void *context, void *n);
    void *(*GetR)(void *context, void *n);
};

void *Zeta_BinTree_StepL(Zeta_BinTreeNodeAccessor *bt_accessor, void *n);

void *Zeta_BinTree_StepR(Zeta_BinTreeNodeAccessor *bt_accessor, void *n);
