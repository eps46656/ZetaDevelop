#pragma once

#include "define.h"

typedef struct {
    void *(*GetP)(void *n);
    void *(*GetL)(void *n);
    void *(*GetR)(void *n);
} Zeta_BinTreeNodeAccessor;

void *Zeta_BinTree_StepL(Zeta_BinTreeNodeAccessor *bt_accessor, void *n);

void *Zeta_BinTree_StepR(Zeta_BinTreeNodeAccessor *bt_accessor, void *n);
