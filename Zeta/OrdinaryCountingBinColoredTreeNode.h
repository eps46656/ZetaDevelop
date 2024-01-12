#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_OrdinaryCountingBinColoredTreeNode) {
    Zeta_OrdinaryCountingBinColoredTreeNode *p;
    Zeta_OrdinaryCountingBinColoredTreeNode *l;
    Zeta_OrdinaryCountingBinColoredTreeNode *r;
    diff_t acc_size;
}
__attribute__((aligned(2)));

void Zeta_OrdinaryCountingBinColoredTreeNode_Init(void *context, void *n);

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetP(void *context, void *n);

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetL(void *context, void *n);

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetR(void *context, void *n);

int Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(void *context, void *n);

void Zeta_OrdinaryCountingBinColoredTreeNode_ReverseColor(void *context,
                                                          void *n);

diff_t Zeta_OrdinaryCountingBinColoredTreeNode_GetAccSize(void *context,
                                                          void *n);

diff_t Zeta_OrdinaryCountingBinColoredTreeNode_GetSize(void *context, void *n);

void Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(void *context, void *n,
                                                     diff_t size);

void Zeta_OrdinaryCountingBinColoredTreeNode_AttachL(void *context, void *n,
                                                     void *m);

void Zeta_OrdinaryCountingBinColoredTreeNode_AttachR(void *context, void *n,
                                                     void *m);

void Zeta_OrdinaryCountingBinColoredTreeNode_Detach(void *context, void *n);

void Zeta_OrdinaryCountingBinColoredTreeNode_Swap(void *context, void *n,
                                                  void *m);

void Zeta_OrdinaryCountingBinColoredTreeNode_RotateL(void *context, void *n);

void Zeta_OrdinaryCountingBinColoredTreeNode_RotateR(void *context, void *n);
