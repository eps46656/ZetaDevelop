#pragma once

#include "BinTree.h"

ZETA_ExternC_Beg;

typedef struct Zeta_RelCntRBTreeNode Zeta_RelCntRBTreeNode;

struct Zeta_RelCntRBTreeNode {
    uintptr_t p;
    uintptr_t l;
    uintptr_t r;
    size_t acc_size;
} __attribute__((aligned(2)));

void Zeta_RelCntRBTreeNode_Init(void* context, void* n);

void* Zeta_RelCntRBTreeNode_GetP(void* context, void* n);
void* Zeta_RelCntRBTreeNode_GetL(void* context, void* n);
void* Zeta_RelCntRBTreeNode_GetR(void* context, void* n);

void Zeta_RelCntRBTreeNode_SetP(void* context, void* n, void* m);
void Zeta_RelCntRBTreeNode_SetL(void* context, void* n, void* m);
void Zeta_RelCntRBTreeNode_SetR(void* context, void* n, void* m);

int Zeta_RelCntRBTreeNode_GetColor(void* context, void* n);
void Zeta_RelCntRBTreeNode_SetColor(void* context, void* n, int p_color);

size_t Zeta_RelCntRBTreeNode_GetAccSize(void* context, void* n);
void Zeta_RelCntRBTreeNode_SetAccSize(void* context, void* n, size_t acc_size);

void Zeta_RelCntRBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr);

ZETA_ExternC_End;
