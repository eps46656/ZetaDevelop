#pragma once

#include "RBTree.h"

ZETA_DECL_STRUCT(Zeta_CountingBinTreeNodeOpr) {
    void *(*GetP)(void *n);
    void *(*GetL)(void *n);
    void *(*GetR)(void *n);

    diff_t (*GetAccSize)(void *n);
    diff_t (*GetSize)(void *n);
    void (*SetSize)(void *n, diff_t size);
};

void Zeta_CountingBinTree_Access(void **dst_n, diff_t *dst_idx,
                                 Zeta_CountingBinTreeNodeOpr *cbtn_opr, void *n,
                                 diff_t idx);

void Zeta_CountingBinTree_GetAccSize(diff_t *dst_l_acc_size,
                                     diff_t *dst_r_acc_size,
                                     Zeta_CountingBinTreeNodeOpr *cbtn_opr,
                                     void *n);

void *Zeta_CountingBinTree_InsertL(Zeta_CountingBinTreeNodeOpr *cbtn_opr,
                                   void *n, void *m);

void *Zeta_CountingBinTree_InsertR(Zeta_CountingBinTreeNodeOpr *cbtn_opr,
                                   void *n, void *m);

void *Zeta_CountingBinTree_Extract(Zeta_CountingBinTreeNodeOpr *cbtn_opr,
                                   void *n);
