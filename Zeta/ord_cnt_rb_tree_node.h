#pragma once

#include "bin_tree.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_OrdCntRBTreeNode);

struct Zeta_OrdCntRBTreeNode {
    void* p;
    Zeta_OrdCntRBTreeNode* l;
    Zeta_OrdCntRBTreeNode* r;
    size_t acc_size;
} __attribute__((aligned(2)));

void Zeta_OrdCntRBTreeNode_Init(void const* context, void* n);

void* Zeta_OrdCntRBTreeNode_GetP(void const* context, void* n);
void* Zeta_OrdCntRBTreeNode_GetL(void const* context, void* n);
void* Zeta_OrdCntRBTreeNode_GetR(void const* context, void* n);

void Zeta_OrdCntRBTreeNode_SetP(void const* context, void* n, void* m);
void Zeta_OrdCntRBTreeNode_SetL(void const* context, void* n, void* m);
void Zeta_OrdCntRBTreeNode_SetR(void const* context, void* n, void* m);

int Zeta_OrdCntRBTreeNode_GetPColor(void const* context, void* n);
void Zeta_OrdCntRBTreeNode_SetPColor(void const* context, void* n, int color);

size_t Zeta_OrdCntRBTreeNode_GetAccSize(void const* context, void* n);
void Zeta_OrdCntRBTreeNode_SetAccSize(void const* context, void* n,
                                      size_t acc_size);

void Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(
    void const* context, Zeta_BinTreeNodeOperator* btn_opr);

extern Zeta_BinTreeNodeOperator const zeta_ord_cnt_rb_tree_node_opr;

ZETA_ExternC_End;
