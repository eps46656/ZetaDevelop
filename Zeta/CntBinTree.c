#include "CntBinTree.h"

#include "utils.h"

void Zeta_CntBinTree_Access(void** dst_n, size_t* dst_idx,
                            Zeta_CntBinTreeNodeOpr* cbtn_opr, void* n,
                            size_t idx) {
    ZETA_DebugAssert(cbtn_opr != NULL);
    ZETA_DebugAssert(cbtn_opr->GetL != NULL);
    ZETA_DebugAssert(cbtn_opr->GetR != NULL);
    ZETA_DebugAssert(cbtn_opr->GetAccSize != NULL);
    ZETA_DebugAssert(cbtn_opr->GetSize != NULL);
    ZETA_DebugAssert(0 <= idx);

    if (dst_n == NULL && dst_idx == NULL) { return; }

    void* context = cbtn_opr->context;

    size_t n_acc_size = cbtn_opr->GetAccSize(context, n);

    if (n_acc_size <= idx) {
        if (dst_n != NULL) { *dst_n = NULL; }
        if (dst_idx != NULL) { *dst_idx = idx - n_acc_size; }
        return;
    }

    while (n != NULL) {
        void* nl = cbtn_opr->GetL(context, n);
        size_t nl_acc_size = cbtn_opr->GetAccSize(context, nl);

        if (idx < nl_acc_size) {
            n = nl;
            continue;
        }

        idx -= nl_acc_size;
        size_t n_size = cbtn_opr->GetSize(context, n);

        if (idx < n_size) { break; }

        n = cbtn_opr->GetR(context, n);
        idx -= n_size;
    }

    if (dst_n != NULL) { *dst_n = n; }
    if (dst_idx != NULL) { *dst_idx = idx; }
}

void Zeta_CntBinTree_GetAccSize(size_t* dst_l_acc_size, size_t* dst_r_acc_size,
                                Zeta_CntBinTreeNodeOpr* cbtn_opr, void* n) {
    ZETA_DebugAssert(cbtn_opr != NULL);
    ZETA_DebugAssert(cbtn_opr->GetP != NULL);
    ZETA_DebugAssert(cbtn_opr->GetL != NULL);
    ZETA_DebugAssert(cbtn_opr->GetR != NULL);
    ZETA_DebugAssert(cbtn_opr->GetAccSize != NULL);
    ZETA_DebugAssert(cbtn_opr->GetSize != NULL);
    ZETA_DebugAssert(n != NULL);

    if (dst_l_acc_size == NULL && dst_r_acc_size == NULL) { return; }

    void* context = cbtn_opr->context;

    size_t l_acc_size =
        cbtn_opr->GetAccSize(context, cbtn_opr->GetL(context, n));
    size_t r_acc_size =
        cbtn_opr->GetAccSize(context, cbtn_opr->GetR(context, n));

    for (;;) {
        void* np = cbtn_opr->GetP(context, n);
        if (np == NULL) { break; }

        size_t k = cbtn_opr->GetAccSize(context, np) -
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
