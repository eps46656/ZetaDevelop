#include "BinHeap.h"

#include "utils.h"

void Zeta_BinHeap_Init(void* bh_) {
    Zeta_BinHeap* bh = bh_;
    ZETA_DebugAssert(bh != NULL);

    bh->size = 0;
    bh->root = NULL;

    ZETA_DebugAssert(bh->btn_opr);
}

size_t Zeta_BinHeap_GetSize(void* bh_) {
    Zeta_BinHeap* bh = bh_;
    ZETA_DebugAssert(bh != NULL);

    return bh->size;
}

void* Zeta_BinHeap_Access(void* bh_, size_t idx) {
    Zeta_BinHeap* bh = bh_;
    ZETA_DebugAssert(bh != NULL);

    ZETA_DebugAssert(idx < bh->size);

    void* context = bh->btn_opr->context;
    void* (*GetL)(void* context, void* n) = bh->btn_opr->GetL;
    void* (*GetR)(void* context, void* n) = bh->btn_opr->GetR;

    void* n = bh->root;

    for (size_t k = idx + 1; 1 < k; k /= 2) {
        ZETA_DebugAssert(n != NULL);

        if (k % 2 == 0) {
            n = GetL(context, n);
        } else {
            n = GetR(context, n);
        }
    }

    ZETA_DebugAssert(n != NULL);

    return n;
}

void Zeta_BinHeap_Insert(void* bh_, void* n) {
    Zeta_BinHeap* bh = bh_;
    ZETA_DebugAssert(bh != NULL);

    ZETA_DebugAssert(n != NULL);

    if (bh->size == 0) {
        bh->size = 1;
        bh->root = n;
        return;
    }

    void* context = bh->btn_opr->context;
    void* (*GetL)(void* context, void* n) = bh->btn_opr->GetL;
    void* (*GetR)(void* context, void* n) = bh->btn_opr->GetR;

    void* pos = bh->root;

    size_t k = bh->size + 1;

    for (; 4 <= k; k = k / 2) {
        ZETA_DebugAssert(pos != NULL);

        if (k % 2 == 0) {
            pos = GetL(context, pos);
        } else {
            pos = GetR(context, pos);
        }
    }

    ZETA_DebugAssert(pos != NULL);

    ++bh->size;

    if (k == 2) {
        Zeta_BinTree_AttatchL(bh->btn_opr, pos, n);
    } else {
        Zeta_BinTree_AttatchR(bh->btn_opr, pos, n);
    }
}

void* Zeta_BinHeap_Extract(void* bh_, void* n) {
    Zeta_BinHeap* bh = bh_;
    ZETA_DebugAssert(bh != NULL);

    ZETA_DebugAssert(n != NULL);

    ZETA_DebugAssert(bh->root == Zeta_GetMostLink(bh->btn_opr->context,
                                                  bh->btn_opr->GetP, n));

    void* m = Zeta_BinHeap_Access(bh, bh->size - 1);

    --bh->size;

    if (n != m) { Zeta_BinTree_Swap(bh->btn_opr, n, m); }

    Zeta_BinTree_Detach(bh->btn_opr, n);

    if (n == m) { m = NULL; }
    if (bh->root == n) { bh->root = m; }

    return m;
}

void Zeta_BinHeap_UpdateRoot(void* bh_) {
    Zeta_BinHeap* bh = bh_;
    ZETA_DebugAssert(bh != NULL);

    bh->root =
        Zeta_GetMostLink(bh->btn_opr->context, bh->btn_opr->GetP, bh->root);
}

static void Check_(size_t size, void* n, Zeta_BinTreeNodeOperator* btn_opr) {
    ZETA_DebugAssert((size == 0) == (n == NULL));

    if (size == 0) { return; }

    void* context = btn_opr->context;
    void* (*GetL)(void* context, void* n) = btn_opr->GetL;
    void* (*GetR)(void* context, void* n) = btn_opr->GetR;

    Check_(size / 2, GetL(context, n), btn_opr);
    Check_((size - 1) / 2, GetR(context, n), btn_opr);
}

void Zeta_BinHeap_Check(void* bh_) {
    Zeta_BinHeap* bh = bh_;
    ZETA_DebugAssert(bh != NULL);

    Check_(bh->size, bh->root, bh->btn_opr);
}
