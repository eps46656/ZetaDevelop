#pragma once

#include "define.h"

ZETA_extern_c_beg;

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

int Zeta_RelCntRBTreeNode_GetColor(void* context, void* n);

void Zeta_RelCntRBTreeNode_ReverseColor(void* context, void* n);

size_t Zeta_RelCntRBTreeNode_GetAccSize(void* context, void* n);

size_t Zeta_RelCntRBTreeNode_GetSize(void* context, void* n);

void Zeta_RelCntRBTreeNode_SetSize(void* context, void* n, size_t size);

void Zeta_RelCntRBTreeNode_AttachL(void* context, void* n, void* m);

void Zeta_RelCntRBTreeNode_AttachR(void* context, void* n, void* m);

void Zeta_RelCntRBTreeNode_Detach(void* context, void* n);

void Zeta_RelCntRBTreeNode_Swap(void* context, void* n, void* m);

void Zeta_RelCntRBTreeNode_RotateL(void* context, void* n);

void Zeta_RelCntRBTreeNode_RotateR(void* context, void* n);

ZETA_extern_c_end;
