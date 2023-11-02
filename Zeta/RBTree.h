#pragma once

#include "BinTree.h"

ZETA_DECL_STRUCT(Zeta_RBTreeNodeOpr) {
    void *(*GetP)(void *n);
    void *(*GetL)(void *n);
    void *(*GetR)(void *n);

    int (*GetColor)(void *n);
    void (*ReverseColor)(void *n);

    void (*AttachL)(void *n, void *m);
    void (*AttachR)(void *n, void *m);

    void (*Detach)(void *n);

    void (*Swap)(void *n, void *m);

    void (*RotateL)(void *n);
    void (*RotateR)(void *n);
};

void *Zeta_RBTree_InsertL(Zeta_RBTreeNodeOpr *rbtn_opr, void *n, void *m);

void *Zeta_RBTree_InsertR(Zeta_RBTreeNodeOpr *rbtn_opr, void *n, void *m);

void *Zeta_RBTree_Extract(Zeta_RBTreeNodeOpr *rbtn_opr, void *n);
