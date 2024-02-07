#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_OrdRBTreeNode Zeta_OrdRBTreeNode;

struct Zeta_OrdRBTreeNode {
    Zeta_OrdRBTreeNode* p;
    Zeta_OrdRBTreeNode* l;
    Zeta_OrdRBTreeNode* r;
} __attribute__((aligned(2)));

void Zeta_OrdRBTreeNode_Init(void* context, void* n);

void* Zeta_OrdRBTreeNode_GetP(void* context, void* n);

void* Zeta_OrdRBTreeNode_GetL(void* context, void* n);

void* Zeta_OrdRBTreeNode_GetR(void* context, void* n);

int Zeta_OrdRBTreeNode_GetColor(void* context, void* n);

void Zeta_OrdRBTreeNode_ReverseColor(void* context, void* n);

void Zeta_OrdRBTreeNode_AttachL(void* context, void* n, void* m);

void Zeta_OrdRBTreeNode_AttachR(void* context, void* n, void* m);

void Zeta_OrdRBTreeNode_Detach(void* context, void* n);

void Zeta_OrdRBTreeNode_Swap(void* context, void* n, void* m);

void Zeta_OrdRBTreeNode_RotateL(void* context, void* n);

void Zeta_OrdRBTreeNode_RotateR(void* context, void* n);

ZETA_extern_c_end;
