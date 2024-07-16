#pragma once

#include "BinTree.h"

ZETA_ExternC_Beg;

ZETA_DeclareStruct(Zeta_OrdCnt3RBTreeNode);

struct Zeta_OrdCnt3RBTreeNode {
    unsigned char* p;
    unsigned char* l;
    unsigned char* r;
    size_t acc_size;
} __attribute__((aligned(2)));

void Zeta_OrdCnt3RBTreeNode_Init(void* context, void* n);

void* Zeta_OrdCnt3RBTreeNode_GetP(void* context, void* n);
void* Zeta_OrdCnt3RBTreeNode_GetL(void* context, void* n);
void* Zeta_OrdCnt3RBTreeNode_GetR(void* context, void* n);

void Zeta_OrdCnt3RBTreeNode_SetP(void* context, void* n, void* m);
void Zeta_OrdCnt3RBTreeNode_SetL(void* context, void* n, void* m);
void Zeta_OrdCnt3RBTreeNode_SetR(void* context, void* n, void* m);

int Zeta_OrdCnt3RBTreeNode_GetPColor(void* context, void* n);
int Zeta_OrdCnt3RBTreeNode_GetLColor(void* context, void* n);
int Zeta_OrdCnt3RBTreeNode_GetRColor(void* context, void* n);

void Zeta_OrdCnt3RBTreeNode_SetPColor(void* context, void* n, int color);
void Zeta_OrdCnt3RBTreeNode_SetLColor(void* context, void* n, int color);
void Zeta_OrdCnt3RBTreeNode_SetRColor(void* context, void* n, int color);

size_t Zeta_OrdCnt3RBTreeNode_GetAccSize(void* context, void* n);
void Zeta_OrdCnt3RBTreeNode_SetAccSize(void* context, void* n, size_t acc_size);

void Zeta_OrdCnt3RBTreeNode_DeployBinTreeNodeOperator(
    void* context, Zeta_BinTreeNodeOperator* btn_opr);

ZETA_ExternC_End;
