#pragma once

#include "define.h"

ZETA_extern_c_beg;

typedef struct Zeta_RelRBTreeNode Zeta_RelRBTreeNode;

struct Zeta_RelRBTreeNode {
    uintptr_t p;
    uintptr_t l;
    uintptr_t r;
};

void Zeta_RelRBTreeNode_Init(void* context, void* n);

void* Zeta_RelRBTreeNode_GetP(void* context, void* n);

void* Zeta_RelRBTreeNode_GetL(void* context, void* n);

void* Zeta_RelRBTreeNode_GetR(void* context, void* n);

int Zeta_RelRBTreeNode_GetColor(void* context, void* n);

void Zeta_RelRBTreeNode_ReverseColor(void* context, void* n);

void Zeta_RelRBTreeNode_AttachL(void* context, void* n, void* m);

void Zeta_RelRBTreeNode_AttachR(void* context, void* n, void* m);

void Zeta_RelRBTreeNode_Detach(void* context, void* n);

void Zeta_RelRBTreeNode_Swap(void* context, void* n, void* m);

void Zeta_RelRBTreeNode_RotateL(void* context, void* n);

void Zeta_RelRBTreeNode_RotateR(void* context, void* n);

ZETA_extern_c_end;
