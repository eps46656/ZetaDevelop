#pragma once

#include "bin_tree.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_OrdRBTreeNode);

struct Zeta_OrdRBTreeNode {
    void* p;
    Zeta_OrdRBTreeNode* l;
    Zeta_OrdRBTreeNode* r;
} __attribute__((aligned(2)));

void Zeta_OrdRBTreeNode_Init(void const* context, void* n);

void* Zeta_OrdRBTreeNode_GetP(void const* context, void* n);
void* Zeta_OrdRBTreeNode_GetL(void const* context, void* n);
void* Zeta_OrdRBTreeNode_GetR(void const* context, void* n);

void Zeta_OrdRBTreeNode_SetP(void const* context, void* n, void* m);
void Zeta_OrdRBTreeNode_SetL(void const* context, void* n, void* m);
void Zeta_OrdRBTreeNode_SetR(void const* context, void* n, void* m);

int Zeta_OrdRBTreeNode_GetPColor(void const* context, void* n);
void Zeta_OrdRBTreeNode_SetPColor(void const* context, void* n, int color);

void Zeta_OrdRBTreeNode_DeployBinTreeNodeOperator(
    void const* context, Zeta_BinTreeNodeOperator* btn_opr);

extern Zeta_BinTreeNodeOperator const zeta_ord_rb_tree_node_opr;

ZETA_ExternC_End;
