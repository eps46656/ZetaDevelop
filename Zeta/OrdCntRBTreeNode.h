#pragma once

#include "BinTree.h"

ZETA_extern_c_beg;

typedef struct Zeta_OrdCntRBTreeNode Zeta_OrdCntRBTreeNode;

struct Zeta_OrdCntRBTreeNode {
    uintptr_t p;
    uintptr_t l;
    uintptr_t r;
    size_t acc_size;
} __attribute__((aligned(2)));

void Zeta_OrdCntRBTreeNode_Init(void* context, void* n);

void* Zeta_OrdCntRBTreeNode_GetP(void* context, void* n);
void* Zeta_OrdCntRBTreeNode_GetL(void* context, void* n);
void* Zeta_OrdCntRBTreeNode_GetR(void* context, void* n);

void Zeta_OrdCntRBTreeNode_SetP(void* context, void* n, void* m);
void Zeta_OrdCntRBTreeNode_SetL(void* context, void* n, void* m);
void Zeta_OrdCntRBTreeNode_SetR(void* context, void* n, void* m);

int Zeta_OrdCntRBTreeNode_GetColor(void* context, void* n);
void Zeta_OrdCntRBTreeNode_SetColor(void* context, void* n, int p_color);

size_t Zeta_OrdCntRBTreeNode_GetAccSize(void* context, void* n);
void Zeta_OrdCntRBTreeNode_SetAccSize(void* context, void* n, size_t acc_size);

void Zeta_OrdCntRBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr);

ZETA_extern_c_end;
