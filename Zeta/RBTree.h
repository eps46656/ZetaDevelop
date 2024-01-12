#pragma once

#include "BinTree.h"

ZETA_DECL_STRUCT(Zeta_RBTreeNodeOpr) {
    void *context;

    void *(*GetP)(void *context, void *n);
    void *(*GetL)(void *context, void *n);
    void *(*GetR)(void *context, void *n);

    int (*GetColor)(void *context, void *n);
    void (*ReverseColor)(void *context, void *n);

    void (*AttachL)(void *context, void *n, void *m);
    void (*AttachR)(void *context, void *n, void *m);

    void (*Detach)(void *context, void *n);

    void (*Swap)(void *context, void *n, void *m);

    void (*RotateL)(void *context, void *n);
    void (*RotateR)(void *context, void *n);
};

void *Zeta_RBTree_InsertL(Zeta_RBTreeNodeOpr *rbtn_opr, void *n, void *m);

void *Zeta_RBTree_InsertR(Zeta_RBTreeNodeOpr *rbtn_opr, void *n, void *m);

void *Zeta_RBTree_Extract(Zeta_RBTreeNodeOpr *rbtn_opr, void *n);
