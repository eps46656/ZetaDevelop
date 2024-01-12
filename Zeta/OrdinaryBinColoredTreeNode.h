#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_OrdinaryBinColoredTreeNode) {
    Zeta_OrdinaryBinColoredTreeNode *p;
    Zeta_OrdinaryBinColoredTreeNode *l;
    Zeta_OrdinaryBinColoredTreeNode *r;
};

void *Zeta_OrdinaryBinColoredTreeNode_GetP(void *context, void *n);

void *Zeta_OrdinaryBinColoredTreeNode_GetL(void *context, void *n);

void *Zeta_OrdinaryBinColoredTreeNode_GetR(void *context, void *n);

int Zeta_OrdinaryBinColoredTreeNode_GetColor(void *context, void *n);

void Zeta_OrdinaryBinColoredTreeNode_ReverseColor(void *context, void *n);

void Zeta_OrdinaryBinColoredTreeNode_AttachL(void *context, void *n, void *m);

void Zeta_OrdinaryBinColoredTreeNode_AttachR(void *context, void *n, void *m);

void Zeta_OrdinaryBinColoredTreeNode_Detach(void *context, void *n);

void Zeta_OrdinaryBinColoredTreeNode_Swap(void *context, void *n, void *m);

void Zeta_OrdinaryBinColoredTreeNode_RotateL(void *context, void *n);

void Zeta_OrdinaryBinColoredTreeNode_RotateR(void *context, void *n);
