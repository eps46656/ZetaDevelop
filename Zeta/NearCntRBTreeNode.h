#pragma once

#include "define.h"

typedef s32_t Zeta_NearCntRBTreeNode_diff_t;

ZETA_DECL_STRUCT(Zeta_NearCntRBTreeNode) {
    Zeta_NearCntRBTreeNode_diff_t p;
    Zeta_NearCntRBTreeNode_diff_t l;
    Zeta_NearCntRBTreeNode_diff_t r;
    size_t acc_size;
}
__attribute__((aligned(2)));

void Zeta_NearCntRBTreeNode_Init(void *context, void *n);

void *Zeta_NearCntRBTreeNode_GetP(void *context, void *n);

void *Zeta_NearCntRBTreeNode_GetL(void *context, void *n);

void *Zeta_NearCntRBTreeNode_GetR(void *context, void *n);

int Zeta_NearCntRBTreeNode_GetColor(void *context, void *n);

void Zeta_NearCntRBTreeNode_ReverseColor(void *context, void *n);

size_t Zeta_NearCntRBTreeNode_GetAccSize(void *context, void *n);

size_t Zeta_NearCntRBTreeNode_GetSize(void *context, void *n);

void Zeta_NearCntRBTreeNode_SetSize(void *context, void *n, size_t size);

void Zeta_NearCntRBTreeNode_AttachL(void *context, void *n, void *m);

void Zeta_NearCntRBTreeNode_AttachR(void *context, void *n, void *m);

void Zeta_NearCntRBTreeNode_Detach(void *context, void *n);

void Zeta_NearCntRBTreeNode_Swap(void *context, void *n, void *m);

void Zeta_NearCntRBTreeNode_RotateL(void *context, void *n);

void Zeta_NearCntRBTreeNode_RotateR(void *context, void *n);
