#include "CountingBinTree.h"
#include "utils.h"

void Zeta_CountingBinTree_Access(void **dst_n, diff_t *dst_idx,
                                 Zeta_CountingBinTreeNodeOpr *cbtn_opr, void *n,
                                 diff_t idx) {
    ZETA_DEBUG_ASSERT(cbtn_opr != NULL);
    ZETA_DEBUG_ASSERT(cbtn_opr->GetL != NULL);
    ZETA_DEBUG_ASSERT(cbtn_opr->GetR != NULL);
    ZETA_DEBUG_ASSERT(cbtn_opr->GetAccSize != NULL);
    ZETA_DEBUG_ASSERT(cbtn_opr->GetSize != NULL);
    ZETA_DEBUG_ASSERT(0 <= idx);

    if (dst_n == NULL && dst_idx == NULL) { return; }

    void *context = cbtn_opr->context;

    diff_t n_acc_size = cbtn_opr->GetAccSize(context, n);

    if (n_acc_size <= idx) {
        if (dst_n != NULL) { *dst_n = NULL; }
        if (dst_idx != NULL) { *dst_idx = idx - n_acc_size; }
        return;
    }

    while (n != NULL) {
        void *nl = cbtn_opr->GetL(context, n);
        diff_t nl_acc_size = cbtn_opr->GetAccSize(context, nl);

        if (idx < nl_acc_size) {
            n = nl;
            continue;
        }

        idx -= nl_acc_size;
        diff_t n_size = cbtn_opr->GetSize(context, n);

        if (idx < n_size) { break; }

        n = cbtn_opr->GetR(context, n);
        idx -= n_size;
    }

    if (dst_n != NULL) { *dst_n = n; }
    if (dst_idx != NULL) { *dst_idx = idx; }
}

void Zeta_CountingBinTree_GetAccSize(diff_t *dst_l_acc_size,
                                     diff_t *dst_r_acc_size,
                                     Zeta_CountingBinTreeNodeOpr *cbtn_opr,
                                     void *n) {
    ZETA_DEBUG_ASSERT(cbtn_opr != NULL);
    ZETA_DEBUG_ASSERT(cbtn_opr->GetP != NULL);
    ZETA_DEBUG_ASSERT(cbtn_opr->GetL != NULL);
    ZETA_DEBUG_ASSERT(cbtn_opr->GetR != NULL);
    ZETA_DEBUG_ASSERT(cbtn_opr->GetAccSize != NULL);
    ZETA_DEBUG_ASSERT(cbtn_opr->GetSize != NULL);
    ZETA_DEBUG_ASSERT(n != NULL);

    if (dst_l_acc_size == NULL && dst_r_acc_size == NULL) { return; }

    void *context = cbtn_opr->context;

    diff_t l_acc_size =
        cbtn_opr->GetAccSize(context, cbtn_opr->GetL(context, n));
    diff_t r_acc_size =
        cbtn_opr->GetAccSize(context, cbtn_opr->GetR(context, n));

    for (;;) {
        void *np = cbtn_opr->GetP(context, n);
        if (np == NULL) { break; }

        diff_t k = cbtn_opr->GetAccSize(context, np) -
                   cbtn_opr->GetAccSize(context, n);

        if (cbtn_opr->GetL(context, np) == n) {
            r_acc_size += k;
        } else {
            l_acc_size += k;
        }

        n = np;
    }

    if (dst_l_acc_size != NULL) { *dst_l_acc_size = l_acc_size; }
    if (dst_r_acc_size != NULL) { *dst_r_acc_size = r_acc_size; }
}

/*
void *Zeta_CountingBinTree_InsertL(Zeta_CountingBinTreeNodeOpr *cbtn_opr,
                                   void *n, void *m) {
    ZETA_DEBUG_ASSERT(cbtn_opr != NULL);

    Zeta_RBTreeNodeOpr rbtn_opr = {
        .GetP = cbtn_opr->GetP,
        .GetL = cbtn_opr->GetL,
        .GetR = cbtn_opr->GetR,

        .GetColor = cbtn_opr->GetColor,
        .ReverseColor = cbtn_opr->ReverseColor,

        .AttachL = cbtn_opr->AttachL,
        .AttachR = cbtn_opr->AttachR,

        .Detach = cbtn_opr->Detach,

        .Swap = cbtn_opr->Swap,

        .RotateL = cbtn_opr->RotateL,
        .RotateR = cbtn_opr->RotateR,
    };

    return Zeta_RBTree_InsertL(&rbtn_opr, n, m);
}

void *Zeta_CountingBinTree_InsertR(Zeta_CountingBinTreeNodeOpr *cbtn_opr,
                                   void *n, void *m) {
    ZETA_DEBUG_ASSERT(cbtn_opr != NULL);

    Zeta_RBTreeNodeOpr rbtn_opr = {
        .GetP = cbtn_opr->GetP,
        .GetL = cbtn_opr->GetL,
        .GetR = cbtn_opr->GetR,

        .GetColor = cbtn_opr->GetColor,
        .ReverseColor = cbtn_opr->ReverseColor,

        .AttachL = cbtn_opr->AttachL,
        .AttachR = cbtn_opr->AttachR,

        .Detach = cbtn_opr->Detach,

        .Swap = cbtn_opr->Swap,

        .RotateL = cbtn_opr->RotateL,
        .RotateR = cbtn_opr->RotateR,
    };

    return Zeta_RBTree_InsertR(&rbtn_opr, n, m);
}

void *Zeta_CountingBinTree_Extract(Zeta_CountingBinTreeNodeOpr *cbtn_opr,
                                   void *n) {
    Zeta_RBTreeNodeOpr rbtn_opr = {
        .GetP = cbtn_opr->GetP,
        .GetL = cbtn_opr->GetL,
        .GetR = cbtn_opr->GetR,

        .GetColor = cbtn_opr->GetColor,
        .ReverseColor = cbtn_opr->ReverseColor,

        .AttachL = cbtn_opr->AttachL,
        .AttachR = cbtn_opr->AttachR,

        .Detach = cbtn_opr->Detach,

        .Swap = cbtn_opr->Swap,

        .RotateL = cbtn_opr->RotateL,
        .RotateR = cbtn_opr->RotateR,
    };

    return Zeta_RBTree_Extract(&rbtn_opr, n);
}
*/
