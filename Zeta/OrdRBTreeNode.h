#pragma once

#include "BinTree.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_OrdRBTreeNode);

struct Zeta_OrdRBTreeNode {
    unsigned char* p;
    Zeta_OrdRBTreeNode* l;
    Zeta_OrdRBTreeNode* r;
} __attribute__((aligned(2)));

void Zeta_OrdRBTreeNode_Init(void* context, void* n);

void* Zeta_OrdRBTreeNode_GetP(void* context, void* n);
void* Zeta_OrdRBTreeNode_GetL(void* context, void* n);
void* Zeta_OrdRBTreeNode_GetR(void* context, void* n);

void Zeta_OrdRBTreeNode_SetP(void* context, void* n, void* m);
void Zeta_OrdRBTreeNode_SetL(void* context, void* n, void* m);
void Zeta_OrdRBTreeNode_SetR(void* context, void* n, void* m);

int Zeta_OrdRBTreeNode_GetColor(void* context, void* n);
void Zeta_OrdRBTreeNode_SetColor(void* context, void* n, int color);

void Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr);

ZETA_ExternC_End;
