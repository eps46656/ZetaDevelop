#pragma once

#include "BinTree.h"

typedef struct Zeta_RBTreeNodeOpr Zeta_RBTreeNodeOpr;

struct Zeta_RBTreeNodeOpr {
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

EXTERN_C void *Zeta_RBTree_InsertL(const Zeta_RBTreeNodeOpr *rbtn_opr, void *n,
                                   void *m);

EXTERN_C void *Zeta_RBTree_InsertR(const Zeta_RBTreeNodeOpr *rbtn_opr, void *n,
                                   void *m);

EXTERN_C void *Zeta_RBTree_Extract(const Zeta_RBTreeNodeOpr *rbtn_opr, void *n);

EXTERN_C size_t Zeta_RBTree_Check(const Zeta_RBTreeNodeOpr *rbtn_opr, void *n);
