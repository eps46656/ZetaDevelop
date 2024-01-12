#include "OrdinaryCountingBinColoredTreeNode.h"

ZETA_STATIC_ASSERT(((intptr_t)NULL & (intptr_t)1) == 0);

void Zeta_OrdinaryCountingBinColoredTreeNode_Init(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    n->p = NULL;
    n->l = NULL;
    n->r = NULL;
    n->acc_size = 0;
}

static Zeta_OrdinaryCountingBinColoredTreeNode *GetP_(void *n_) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return (void *)((intptr_t)(void *)n->p & ~(intptr_t)1);
}

static void SetP_(void *n_, void *p) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    n->p = (void *)((intptr_t)p | ((intptr_t)(void *)n->p & (intptr_t)1));
}

static void AddDiffSize_(Zeta_OrdinaryCountingBinColoredTreeNode *n,
                         diff_t diff_size) {
    if (diff_size == 0) { return; }
    for (; n != NULL; n = GetP_(n)) { n->acc_size += diff_size; }
}

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetP(void *context, void *n_) {
    ZETA_UNUSED(context);
    return GetP_(n_);
}

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetL(void *context, void *n_) {
    ZETA_UNUSED(context);
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return n->l;
}

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetR(void *context, void *n_) {
    ZETA_UNUSED(context);
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return n->r;
}

int Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    return n == NULL ? 0 : (intptr_t)(void *)n->p & (intptr_t)1;
}

void Zeta_OrdinaryCountingBinColoredTreeNode_ReverseColor(void *context,
                                                          void *n_) {
    ZETA_UNUSED(context);
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    n->p = (void *)((intptr_t)(void *)n->p ^ (intptr_t)1);
}

static diff_t GetAccSize_(void *n_) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    return n == NULL ? 0 : n->acc_size;
}

diff_t Zeta_OrdinaryCountingBinColoredTreeNode_GetAccSize(void *context,
                                                          void *n) {
    ZETA_UNUSED(context);
    return GetAccSize_(n);
}

static diff_t GetSize_(void *n_) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;

    return n == NULL ? 0 : n->acc_size - GetAccSize_(n->l) - GetAccSize_(n->r);
}

diff_t Zeta_OrdinaryCountingBinColoredTreeNode_GetSize(void *context, void *n) {
    ZETA_UNUSED(context);
    return GetSize_(n);
}

static void SetSize_(void *n_, diff_t size) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    AddDiffSize_(n, size - GetSize_(n));
}

void Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(void *context, void *n,
                                                     diff_t size) {
    ZETA_UNUSED(context);
    SetSize_(n, size);
}

void Zeta_OrdinaryCountingBinColoredTreeNode_AttachL(void *context, void *n_,
                                                     void *m_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    Zeta_OrdinaryCountingBinColoredTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(n->l == NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(GetP_(m) == NULL);

    n->l = m;
    SetP_(m, n);
    AddDiffSize_(n, m->acc_size);
}

void Zeta_OrdinaryCountingBinColoredTreeNode_AttachR(void *context, void *n_,
                                                     void *m_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    Zeta_OrdinaryCountingBinColoredTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(n->r == NULL);
    ZETA_DEBUG_ASSERT(m != NULL);
    ZETA_DEBUG_ASSERT(GetP_(m) == NULL);

    n->r = m;
    SetP_(m, n);
    AddDiffSize_(n, m->acc_size);
}

void Zeta_OrdinaryCountingBinColoredTreeNode_Detach(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_OrdinaryCountingBinColoredTreeNode *np = GetP_(n);

    if (np == NULL) { return; }

    if (np->l == n) {
        np->l = NULL;
    } else {
        np->r = NULL;
    }

    SetP_(n, NULL);
    AddDiffSize_(np, -n->acc_size);
}

static void Replace_(void *n_, void *m_) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    Zeta_OrdinaryCountingBinColoredTreeNode *m = m_;

    Zeta_OrdinaryCountingBinColoredTreeNode *np = GetP_(n);
    Zeta_OrdinaryCountingBinColoredTreeNode *nl = n->l;
    Zeta_OrdinaryCountingBinColoredTreeNode *nr = n->r;

    m->p = n->p;
    m->l = nl;
    m->r = nr;

    if (np != NULL) {
        if (np->l == n) {
            np->l = m;
        } else {
            np->r = m;
        }
    }

    if (nl != NULL) { SetP_(nl, m); }
    if (nr != NULL) { SetP_(nr, m); }
}

void Zeta_OrdinaryCountingBinColoredTreeNode_Swap(void *context, void *n_,
                                                  void *m_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    Zeta_OrdinaryCountingBinColoredTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(m != NULL);

    if (n == m) { return; }

    diff_t n_size = GetSize_(n);
    diff_t m_size = GetSize_(m);

    Zeta_OrdinaryCountingBinColoredTreeNode tmp;

    Replace_(n, &tmp);
    Replace_(m, n);
    Replace_(&tmp, m);

    diff_t acc_size_tmp = n->acc_size;
    n->acc_size = m->acc_size;
    m->acc_size = acc_size_tmp;

    void *np = GetP_(n);
    void *mp = GetP_(m);

    if (np != NULL) { SetSize_(np, n_size); }
    if (mp != NULL) { SetSize_(mp, m_size); }
}

void Zeta_OrdinaryCountingBinColoredTreeNode_RotateL(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_OrdinaryCountingBinColoredTreeNode *nr = n->r;
    ZETA_DEBUG_ASSERT(nr != NULL);

    Zeta_OrdinaryCountingBinColoredTreeNode *nrl = nr->l;
    Zeta_OrdinaryCountingBinColoredTreeNode *np = GetP_(n);

    if (np != NULL) {
        if (np->l == n) {
            np->l = nr;
        } else {
            np->r = nr;
        }
    }

    SetP_(nr, np);

    nr->l = n;
    SetP_(n, nr);

    diff_t n_acc_size = n->acc_size;
    diff_t new_n_acc_size = n_acc_size - nr->acc_size;
    nr->acc_size = n_acc_size;

    n->r = nrl;

    if (nrl != NULL) {
        SetP_(nrl, n);
        new_n_acc_size += nrl->acc_size;
    }

    n->acc_size = new_n_acc_size;
}

void Zeta_OrdinaryCountingBinColoredTreeNode_RotateR(void *context, void *n_) {
    ZETA_UNUSED(context);

    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);

    Zeta_OrdinaryCountingBinColoredTreeNode *nl = n->l;
    ZETA_DEBUG_ASSERT(nl != NULL);

    Zeta_OrdinaryCountingBinColoredTreeNode *nlr = nl->r;
    Zeta_OrdinaryCountingBinColoredTreeNode *np = GetP_(n);

    if (np != NULL) {
        if (np->l == n) {
            np->l = nl;
        } else {
            np->r = nl;
        }
    }

    SetP_(nl, np);

    nl->r = n;
    SetP_(n, nl);

    diff_t n_acc_size = n->acc_size;
    diff_t new_n_acc_size = n_acc_size - nl->acc_size;
    nl->acc_size = n_acc_size;

    n->l = nlr;

    if (nlr != NULL) {
        SetP_(nlr, n);
        new_n_acc_size += nlr->acc_size;
    }

    n->acc_size = new_n_acc_size;
}
