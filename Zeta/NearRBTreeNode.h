#pragma once

#include "define.h"

typedef s32_t Zeta_NearRBTreeNode_diff_t;

ZETA_DECL_STRUCT(Zeta_NearRBTreeNode) {
    Zeta_NearRBTreeNode_diff_t p;
    Zeta_NearRBTreeNode_diff_t l;
    Zeta_NearRBTreeNode_diff_t r;
}
__attribute__((aligned(2)));

void Zeta_NearRBTreeNode_Init(void *context, void *n);

void *Zeta_NearRBTreeNode_GetP(void *context, void *n);

void *Zeta_NearRBTreeNode_GetL(void *context, void *n);

void *Zeta_NearRBTreeNode_GetR(void *context, void *n);

int Zeta_NearRBTreeNode_GetColor(void *context, void *n);

void Zeta_NearRBTreeNode_ReverseColor(void *context, void *n);

void Zeta_NearRBTreeNode_AttachL(void *context, void *n, void *m);

void Zeta_NearRBTreeNode_AttachR(void *context, void *n, void *m);

void Zeta_NearRBTreeNode_Detach(void *context, void *n);

void Zeta_NearRBTreeNode_Swap(void *context, void *n, void *m);

void Zeta_NearRBTreeNode_RotateL(void *context, void *n);

void Zeta_NearRBTreeNode_RotateR(void *context, void *n);
