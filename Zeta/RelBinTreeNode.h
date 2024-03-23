#pragma once

#include "BinTree.h"

ZETA_extern_c_beg;

typedef struct Zeta_RelBinTreeNode Zeta_RelBinTreeNode;

struct Zeta_RelBinTreeNode {
    uintptr_t p;
    uintptr_t l;
    uintptr_t r;
};

void Zeta_RelBinTreeNode_Init(void* context, void* n);

void* Zeta_RelBinTreeNode_GetP(void* context, void* n);

void* Zeta_RelBinTreeNode_GetL(void* context, void* n);

void* Zeta_RelBinTreeNode_GetR(void* context, void* n);

void Zeta_RelBinTreeNode_SetP(void* context, void* n, void* m);

void Zeta_RelBinTreeNode_SetL(void* context, void* n, void* m);

void Zeta_RelBinTreeNode_SetR(void* context, void* n, void* m);

void Zeta_RelCntRBTreeNode_ToBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr);

ZETA_extern_c_end;
