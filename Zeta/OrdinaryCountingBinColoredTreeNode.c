#include "OrdinaryCountingBinColoredTreeNode.h"

ZETA_STATIC_ASSERT(((uintptr_t)NULL & (uintptr_t)1) == 0);

void Zeta_OrdinaryCountingBinColoredTreeNode_Init(void *n_) {
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
    return (void *)((uintptr_t)(void *)n->p & ~(uintptr_t)1);
}

static void SetP_(void *n_, void *p) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    n->p = (void *)((uintptr_t)p | ((uintptr_t)(void *)n->p & (uintptr_t)1));
}

static void AddDiffSize_(Zeta_OrdinaryCountingBinColoredTreeNode *n,
                         diff_t diff_size) {
    if (diff_size == 0) { return; }
    for (; n != NULL; n = GetP_(n)) { n->acc_size += diff_size; }
}

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetP(void *n_) {
    return GetP_(n_);
}

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetL(void *n_) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return n->l;
}

void *Zeta_OrdinaryCountingBinColoredTreeNode_GetR(void *n_) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    return n->r;
}

int Zeta_OrdinaryCountingBinColoredTreeNode_GetColor(void *n_) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    return n == NULL ? 0 : (uintptr_t)(void *)n->p & (uintptr_t)1;
}

void Zeta_OrdinaryCountingBinColoredTreeNode_ReverseColor(void *n_) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    n->p = (void *)((uintptr_t)(void *)n->p ^ (uintptr_t)1);
}

diff_t Zeta_OrdinaryCountingBinColoredTreeNode_GetAccSize(void *n_) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    return n == NULL ? 0 : n->acc_size;
}

diff_t Zeta_OrdinaryCountingBinColoredTreeNode_GetSize(void *n_) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;

    return n == NULL
               ? 0
               : n->acc_size -
                     Zeta_OrdinaryCountingBinColoredTreeNode_GetAccSize(n->l) -
                     Zeta_OrdinaryCountingBinColoredTreeNode_GetAccSize(n->r);
}

void Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(void *n_, diff_t size) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    ZETA_DEBUG_ASSERT(n != NULL);
    AddDiffSize_(n, size - Zeta_OrdinaryCountingBinColoredTreeNode_GetSize(n));
}

void Zeta_OrdinaryCountingBinColoredTreeNode_AttachL(void *n_, void *m_) {
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

void Zeta_OrdinaryCountingBinColoredTreeNode_AttachR(void *n_, void *m_) {
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

void Zeta_OrdinaryCountingBinColoredTreeNode_Detach(void *n_) {
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

void Zeta_OrdinaryCountingBinColoredTreeNode_Swap(void *n_, void *m_) {
    Zeta_OrdinaryCountingBinColoredTreeNode *n = n_;
    Zeta_OrdinaryCountingBinColoredTreeNode *m = m_;

    ZETA_DEBUG_ASSERT(n != NULL);
    ZETA_DEBUG_ASSERT(m != NULL);

    if (n == m) { return; }

    diff_t n_size = Zeta_OrdinaryCountingBinColoredTreeNode_GetSize(n);
    diff_t m_size = Zeta_OrdinaryCountingBinColoredTreeNode_GetSize(m);

    Zeta_OrdinaryCountingBinColoredTreeNode tmp;

    Replace_(n, &tmp);
    Replace_(m, n);
    Replace_(&tmp, m);

    ZETA_SWAP(n->acc_size, m->acc_size);

    void *np = GetP_(n);
    void *mp = GetP_(m);

    if (np != NULL) {
        Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(np, n_size);
    }

    if (mp != NULL) {
        Zeta_OrdinaryCountingBinColoredTreeNode_SetSize(mp, m_size);
    }
}

void Zeta_OrdinaryCountingBinColoredTreeNode_RotateL(void *n_) {
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

void Zeta_OrdinaryCountingBinColoredTreeNode_RotateR(void *n_) {
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
