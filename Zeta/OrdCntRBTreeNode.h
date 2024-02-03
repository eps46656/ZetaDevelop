#pragma once

#include "define.h"

typedef struct Zeta_OrdCntRBTreeNode Zeta_OrdCntRBTreeNode;

struct Zeta_OrdCntRBTreeNode {
    Zeta_OrdCntRBTreeNode *p;
    Zeta_OrdCntRBTreeNode *l;
    Zeta_OrdCntRBTreeNode *r;
    size_t acc_size;
} __attribute__((aligned(2)));

EXTERN_C void Zeta_OrdCntRBTreeNode_Init(void *context, void *n);

EXTERN_C void *Zeta_OrdCntRBTreeNode_GetP(void *context, void *n);

EXTERN_C void *Zeta_OrdCntRBTreeNode_GetL(void *context, void *n);

EXTERN_C void *Zeta_OrdCntRBTreeNode_GetR(void *context, void *n);

EXTERN_C int Zeta_OrdCntRBTreeNode_GetColor(void *context, void *n);

EXTERN_C void Zeta_OrdCntRBTreeNode_ReverseColor(void *context, void *n);

EXTERN_C size_t Zeta_OrdCntRBTreeNode_GetAccSize(void *context, void *n);

EXTERN_C size_t Zeta_OrdCntRBTreeNode_GetSize(void *context, void *n);

EXTERN_C void Zeta_OrdCntRBTreeNode_SetSize(void *context, void *n,
                                            size_t size);

EXTERN_C void Zeta_OrdCntRBTreeNode_AttachL(void *context, void *n, void *m);

EXTERN_C void Zeta_OrdCntRBTreeNode_AttachR(void *context, void *n, void *m);

EXTERN_C void Zeta_OrdCntRBTreeNode_Detach(void *context, void *n);

EXTERN_C void Zeta_OrdCntRBTreeNode_Swap(void *context, void *n, void *m);

EXTERN_C void Zeta_OrdCntRBTreeNode_RotateL(void *context, void *n);

EXTERN_C void Zeta_OrdCntRBTreeNode_RotateR(void *context, void *n);
