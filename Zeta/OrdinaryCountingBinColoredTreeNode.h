#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_OrdinaryCountingBinColoredTreeNode) {
    Zeta_OrdinaryCountingBinColoredTreeNode *p;
    Zeta_OrdinaryCountingBinColoredTreeNode *l;
    Zeta_OrdinaryCountingBinColoredTreeNode *r;
    diff_t acc_size;
}
__attribute__((aligned(2)));

void Zeta_OrdinaryCountingBinColoredTreeNode_Init(void *n);

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetP(void *n);

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetL(void *n);

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetR(void *n);

int Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(void *n);

void Zeta_OrdinaryCountingBinColoredTreeNode_ReverseColor(void *n);

diff_t Zeta_OrdinaryCountingBinColoredTreeNode_GetAccSize(void *n);

diff_t Zeta_OrdinaryCountingBinColoredTreeNode_GetSize(void *n);

void Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(void *n, diff_t size);

void Zeta_OrdinaryCountingBinColoredTreeNode_AttachL(void *n, void *m);

void Zeta_OrdinaryCountingBinColoredTreeNode_AttachR(void *n, void *m);

void Zeta_OrdinaryCountingBinColoredTreeNode_Detach(void *n);

void Zeta_OrdinaryCountingBinColoredTreeNode_Swap(void *n, void *m);

void Zeta_OrdinaryCountingBinColoredTreeNode_RotateL(void *n);

void Zeta_OrdinaryCountingBinColoredTreeNode_RotateR(void *n);
