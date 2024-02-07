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

void* Zeta_BinTree_StepL(Zeta_BinTreeNodeAccessor* bt_accessor, void* n);

void* Zeta_BinTree_StepR(Zeta_BinTreeNodeAccessor* bt_accessor, void* n);

void* Zeta_BinTree_FindFirst(Zeta_BinTreeNodeAccessor* bt_accessor, void* root,
                             void* target, void* cmp_context,
                             int (*Compare)(void* cmp_context, void* x,
                                            void* y));

ZETA_extern_c_end;
