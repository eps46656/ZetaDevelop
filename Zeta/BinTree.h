#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_BinTreeNodeAccessor Zeta_BinTreeNodeAccessor;

struct Zeta_BinTreeNodeAccessor {
    void* context;

    void* (*GetP)(void* context, void* n);
    void* (*GetL)(void* context, void* n);
    void* (*GetR)(void* context, void* n);
};

void* Zeta_BinTree_StepL(const Zeta_BinTreeNodeAccessor* btn_accessor, void* n);

void* Zeta_BinTree_StepR(const Zeta_BinTreeNodeAccessor* btn_accessor, void* n);

ZETA_extern_c_end;
