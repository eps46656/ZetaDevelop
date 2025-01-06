#pragma once

#include "bin_tree.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_OrdCnt3RBTreeNode);

struct Zeta_OrdCnt3RBTreeNode {
    void* p;
    void* l;
    void* r;
    size_t acc_size;
} __attribute__((aligned(2)));

void Zeta_OrdCnt3RBTreeNode_Init(void const* context, void* n);

void* Zeta_OrdCnt3RBTreeNode_GetP(void const* context, void* n);
void* Zeta_OrdCnt3RBTreeNode_GetL(void const* context, void* n);
void* Zeta_OrdCnt3RBTreeNode_GetR(void const* context, void* n);

void Zeta_OrdCnt3RBTreeNode_SetP(void const* context, void* n, void* m);
void Zeta_OrdCnt3RBTreeNode_SetL(void const* context, void* n, void* m);
void Zeta_OrdCnt3RBTreeNode_SetR(void const* context, void* n, void* m);

int Zeta_OrdCnt3RBTreeNode_GetPColor(void const* context, void* n);
int Zeta_OrdCnt3RBTreeNode_GetLColor(void const* context, void* n);
int Zeta_OrdCnt3RBTreeNode_GetRColor(void const* context, void* n);

void Zeta_OrdCnt3RBTreeNode_SetPColor(void const* context, void* n, int color);
void Zeta_OrdCnt3RBTreeNode_SetLColor(void const* context, void* n, int color);
void Zeta_OrdCnt3RBTreeNode_SetRColor(void const* context, void* n, int color);

size_t Zeta_OrdCnt3RBTreeNode_GetAccSize(void const* context, void* n);
void Zeta_OrdCnt3RBTreeNode_SetAccSize(void const* context, void* n,
                                       size_t acc_size);

void Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(
    void const* context, Zeta_BinTreeNodeOperator* btn_opr);

extern Zeta_BinTreeNodeOperator const zeta_ord_cnt_3rb_tree_node_opr;

ZETA_ExternC_End;
