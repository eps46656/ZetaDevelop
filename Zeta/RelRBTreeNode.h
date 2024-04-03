#pragma once

#include "BinTree.h"

ZETA_ExternC_Beg;

typedef struct Zeta_RelRBTreeNode Zeta_RelRBTreeNode;

struct Zeta_RelRBTreeNode {
    uintptr_t p;
    uintptr_t l;
    uintptr_t r;
} __attribute__((aligned(2)));

void Zeta_RelRBTreeNode_Init(void* context, void* n);

void* Zeta_RelRBTreeNode_GetP(void* context, void* n);
void* Zeta_RelRBTreeNode_GetL(void* context, void* n);
void* Zeta_RelRBTreeNode_GetR(void* context, void* n);

void Zeta_RelRBTreeNode_SetP(void* context, void* n, void* m);
void Zeta_RelRBTreeNode_SetL(void* context, void* n, void* m);
void Zeta_RelRBTreeNode_SetR(void* context, void* n, void* m);

int Zeta_RelRBTreeNode_GetColor(void* context, void* n);

void Zeta_RelRBTreeNode_SetColor(void* context, void* n, int p_color);

void Zeta_RelRBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr);

ZETA_ExternC_End;
