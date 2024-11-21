#pragma once

#include "bin_tree.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_OrdRBTreeNode);

struct Zeta_OrdRBTreeNode {
    void* p;
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

int Zeta_OrdRBTreeNode_GetPColor(void* context, void* n);
void Zeta_OrdRBTreeNode_SetPColor(void* context, void* n, int color);

void Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr);

extern Zeta_BinTreeNodeOperator const zeta_ord_rb_tree_node_opr;

ZETA_ExternC_End;
