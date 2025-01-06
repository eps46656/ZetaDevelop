#pragma once

#include "bin_tree.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_OrdBinTreeNode);

struct Zeta_OrdBinTreeNode {
    Zeta_OrdBinTreeNode* p;
    Zeta_OrdBinTreeNode* l;
    Zeta_OrdBinTreeNode* r;
};

void Zeta_OrdBinTreeNode_Init(void const* context, void* n);

void* Zeta_OrdBinTreeNode_GetP(void const* context, void* n);
void* Zeta_OrdBinTreeNode_GetL(void const* context, void* n);
void* Zeta_OrdBinTreeNode_GetR(void const* context, void* n);

void Zeta_OrdBinTreeNode_SetP(void const* context, void* n, void* m);
void Zeta_OrdBinTreeNode_SetL(void const* context, void* n, void* m);
void Zeta_OrdBinTreeNode_SetR(void const* context, void* n, void* m);

void Zeta_OrdBinTreeNode_DeployBinTreeNodeOperator(
    void const* context, Zeta_BinTreeNodeOperator* btn_opr);

ZETA_ExternC_End;
