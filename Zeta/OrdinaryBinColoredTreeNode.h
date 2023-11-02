#pragma once

#include "define.h"

ZETA_DECL_STRUCT(Zeta_OrdinaryBinColoredTreeNode) {
    Zeta_OrdinaryBinColoredTreeNode *p;
    Zeta_OrdinaryBinColoredTreeNode *l;
    Zeta_OrdinaryBinColoredTreeNode *r;
};

void *Zeta_OrdinaryBinColoredTreeNode_GetP(void *n);

void *Zeta_OrdinaryBinColoredTreeNode_GetL(void *n);

void *Zeta_OrdinaryBinColoredTreeNode_GetR(void *n);

int Zeta_OrdinaryBinColoredTreeNode_GetColor(void *n);

void Zeta_OrdinaryBinColoredTreeNode_ReverseColor(void *n);

void Zeta_OrdinaryBinColoredTreeNode_AttachL(void *n, void *m);

void Zeta_OrdinaryBinColoredTreeNode_AttachR(void *n, void *m);

void Zeta_OrdinaryBinColoredTreeNode_Detach(void *n);

void Zeta_OrdinaryBinColoredTreeNode_Swap(void *n, void *m);

void Zeta_OrdinaryBinColoredTreeNode_RotateL(void *n);

void Zeta_OrdinaryBinColoredTreeNode_RotateR(void *n);
