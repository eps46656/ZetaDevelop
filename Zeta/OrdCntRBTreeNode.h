#pragma once

#include "define.h"

ZETA_extern_c_beg;

    typedef struct Zeta_OrdCntRBTreeNode Zeta_OrdCntRBTreeNode;

struct Zeta_OrdCntRBTreeNode {
    Zeta_OrdCntRBTreeNode* p;
    Zeta_OrdCntRBTreeNode* l;
    Zeta_OrdCntRBTreeNode* r;
    size_t acc_size;
} __attribute__((aligned(2)));

void Zeta_OrdCntRBTreeNode_Init(void* context, void* n);

void* Zeta_OrdCntRBTreeNode_GetP(void* context, void* n);

void* Zeta_OrdCntRBTreeNode_GetL(void* context, void* n);

void* Zeta_OrdCntRBTreeNode_GetR(void* context, void* n);

int Zeta_OrdCntRBTreeNode_GetColor(void* context, void* n);

void Zeta_OrdCntRBTreeNode_ReverseColor(void* context, void* n);

size_t Zeta_OrdCntRBTreeNode_GetAccSize(void* context, void* n);

size_t Zeta_OrdCntRBTreeNode_GetSize(void* context, void* n);

void Zeta_OrdCntRBTreeNode_SetSize(void* context, void* n, size_t size);

void Zeta_OrdCntRBTreeNode_AttachL(void* context, void* n, void* m);

void Zeta_OrdCntRBTreeNode_AttachR(void* context, void* n, void* m);

void Zeta_OrdCntRBTreeNode_Detach(void* context, void* n);

void Zeta_OrdCntRBTreeNode_Swap(void* context, void* n, void* m);

void Zeta_OrdCntRBTreeNode_RotateL(void* context, void* n);

void Zeta_OrdCntRBTreeNode_RotateR(void* context, void* n);

ZETA_extern_c_end;
