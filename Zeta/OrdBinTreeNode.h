#pragma once

#include "BinTree.h"

ZETA_ExternC_Beg;

typedef struct Zeta_OrdBinTreeNode Zeta_OrdBinTreeNode;

struct Zeta_OrdBinTreeNode {
    Zeta_OrdBinTreeNode* p;
    Zeta_OrdBinTreeNode* l;
    Zeta_OrdBinTreeNode* r;
};

void Zeta_OrdBinTreeNode_Init(void* context, void* n);

void* Zeta_OrdBinTreeNode_GetP(void* context, void* n);
void* Zeta_OrdBinTreeNode_GetL(void* context, void* n);
void* Zeta_OrdBinTreeNode_GetR(void* context, void* n);

void Zeta_OrdBinTreeNode_SetP(void* context, void* n, void* m);
void Zeta_OrdBinTreeNode_SetL(void* context, void* n, void* m);
void Zeta_OrdBinTreeNode_SetR(void* context, void* n, void* m);

void Zeta_OrdBinTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr);

ZETA_ExternC_End;
